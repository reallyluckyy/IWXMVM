#pragma once
#include "StdInclude.hpp"
#include "Mod.hpp"

namespace IWXMVM::Signatures
{
    namespace Lambdas
    {
        inline auto IsAbsoluteJumpOrCall = [](std::uintptr_t address) {
            static constexpr std::uint8_t JUMP_OPCODE = 0xE9;
            static constexpr std::uint8_t CALL_OPCODE = 0xE8;

            if (*reinterpret_cast<std::uint8_t*>(address) == JUMP_OPCODE ||
                *reinterpret_cast<std::uint8_t*>(address) == CALL_OPCODE)
                return true;
            else
                return false;
        };

        inline auto DereferenceAddress = [](std::uintptr_t address) {
            return *reinterpret_cast<std::uintptr_t*>(address);
        };

        inline auto DereferenceCallOffset = [](std::uintptr_t address) {
            return *reinterpret_cast<std::uintptr_t*>(address + 1) + 5;
        };

        inline auto FollowCodeFlow = [](std::uintptr_t address) {
            const std::uintptr_t orgAddress = address;

            while (IsAbsoluteJumpOrCall(address))
                address += DereferenceCallOffset(address);

            return (address == orgAddress) ? 0 : address;
        };
    }  // namespace Lambdas

    inline constexpr std::uint16_t maskValue = UINT8_MAX + 1;

    enum struct GameAddressType : std::uint8_t
    {
        Data = 4,
        Code = 5
    };

    inline constexpr void CheckMaskCountAndOffset(const auto& bytes, std::size_t frontMaskCount, std::intptr_t offset,
                                                  GameAddressType type)
    {
        const std::size_t unsOffset = (offset >= 0) ? static_cast<std::size_t>(offset) : 0 - offset;

        if (unsOffset >= bytes.size())
            return;

        const std::size_t requiredMaskCount = static_cast<std::size_t>(type);

        if (offset >= 0)
        {
            std::size_t count = 0;

            for (std::size_t i = unsOffset; i < bytes.size() && i < unsOffset + requiredMaskCount; ++i)
            {
                if (bytes[i] == maskValue)
                    ++count;
            }

            if (count < requiredMaskCount)
                throw std::runtime_error("Incorrect signature.");
        }
        else
        {
            if (unsOffset >= requiredMaskCount)
                return;

            if (unsOffset + frontMaskCount < requiredMaskCount)
                throw std::runtime_error("Incorrect signature.");
        }
    }

    inline constexpr std::size_t GetFrontMaskCount(const auto& bytes)
    {
        std::size_t count = 0;

        for (const auto byte : bytes)
        {
            if (byte == maskValue)
                ++count;
            else
                break;
        }

        if (count >= bytes.size())
            throw std::runtime_error("Number of masks equals or exceeds total length of signature.");

        return count;
    };

    inline constexpr std::size_t HexToDecimal(char c)
    {
        constexpr std::array<std::uint8_t, 4> boundaries{'0', '9', 'A', 'F'};
        const std::uint8_t val = static_cast<std::uint8_t>(c);

        if (val >= boundaries[0] && val <= boundaries[1])  // 0-9
            return val - boundaries[0];

        if (val >= boundaries[2] && val <= boundaries[3])  // A-F
            return val - boundaries[2] + 10;

        throw std::runtime_error("Not a hexadecimal value.");
    }

    template <std::size_t size>
    constexpr auto ConvertStringToBytes(const char* byteString)
    {
        std::array<std::uint16_t, (size + 2) / 3> bytes;
        bytes.fill(maskValue + 1);

        if constexpr (bytes.size() <= 4)
            throw std::runtime_error("Signature length is too short.");

        for (std::size_t strIndex = 0, byteIndex = 0; strIndex + 1 < size;)
        {
            if (byteString[strIndex] == ' ')
            {
                ++strIndex;
                continue;
            }

            if (byteString[strIndex] == '?')
            {
                bytes[byteIndex] = maskValue;

                if (byteString[strIndex + 1] == '?')
                    ++strIndex;
                else
                    throw std::runtime_error("Single mask character detected.");

                ++byteIndex, ++strIndex;
                continue;
            }

            if (byteIndex >= (size + 2) / 3)
                throw std::runtime_error("Incorrect signature input.");

            bytes[byteIndex++] =
                static_cast<uint16_t>((HexToDecimal(byteString[strIndex]) << 4) + HexToDecimal(byteString[strIndex + 1]));
            strIndex += 2;
        }

        if (std::find(bytes.begin(), bytes.end(), maskValue + 1) != bytes.end())
            throw std::runtime_error("Signature parsed incorrectly.");

        if (bytes.back() == maskValue)
            throw std::runtime_error("Incorrect signature input.");

        return bytes;
    }

    inline std::uintptr_t SignatureScanner(const auto& signature, const auto& moduleHandles)
    {
        for (const auto handle : moduleHandles)
        {
            MODULEINFO process{};

            if (!::GetModuleInformation(::GetCurrentProcess(), handle, &process, sizeof(process)) ||
                !process.lpBaseOfDll)
                return 0;

            const std::uintptr_t endOfDll = reinterpret_cast<std::uintptr_t>(process.lpBaseOfDll) + process.SizeOfImage;

            for (std::uintptr_t i = reinterpret_cast<std::uintptr_t>(process.lpBaseOfDll); i < endOfDll; ++i)
            {
                std::size_t j = signature._frontMaskCount;

                for (; j < signature._bytes.size(); ++j)
                {
                    if (signature._bytes[j] != maskValue &&
                        signature._bytes[j] != *reinterpret_cast<std::uint8_t*>(i + j))
                        break;
                }

                if (j == signature._bytes.size())
                    return i + signature._offset;
            }
        }

        return 0;
    }

    using callable_t = decltype([]() {});

    template <std::size_t size, typename Callable = callable_t>
    struct SignatureImpl
    {
        constexpr SignatureImpl(const char (&str)[size], GameAddressType type, std::intptr_t offset = 0,
                                Callable callable = callable_t{}) noexcept
            : _string(std::to_array(str)),
              _bytes(ConvertStringToBytes<size>(str)),
              _frontMaskCount(GetFrontMaskCount(_bytes)),
              _offset(offset),
              _callable(callable)
        {
            CheckMaskCountAndOffset(_bytes, _frontMaskCount, _offset, type);

            static_assert(size > 0);
        }

        auto Scan(const auto& moduleHandles) const
        {
            const std::uintptr_t address = SignatureScanner(*this, moduleHandles);

            if (address == 0)
                throw std::runtime_error(std::format("Failed to find signature:\n\t {}", _string.data()));
            else
            {
                if constexpr (requires { std::declval<Callable>()(address); })
                {
                    try
                    {
                        const std::uintptr_t newAddress = _callable(address);
                        if (newAddress == 0)
                            throw std::runtime_error(std::format(
                                "Failed to find correct game address (1), signature:\n\t {}", _string.data()));

                        return newAddress;
                    }
                    catch (...)
                    {
                        throw std::runtime_error(
                            std::format("Failed to find correct game address (2), signature:\n\t {}", _string.data()));
                    }

                    return std::uintptr_t{};
                }
                else
                    return address;
            }
        }

        std::array<char, size> _string{};
        std::array<std::uint16_t, (size + 2) / 3> _bytes{};
        std::size_t _frontMaskCount{};
        std::intptr_t _offset{};
        Callable _callable;
    };

    template <auto intSignature, Types::ModuleType type = Types::ModuleType::BaseModule>
    struct Signature
    {
        constexpr Signature()
        {
            if (const auto modules = Mod::GetGameInterface()->GetModuleHandles(type); modules.has_value())
                _address = _signature.Scan(modules.value());
        }

        static constexpr auto _signature = intSignature;
        std::uintptr_t _address{};

        std::uintptr_t operator()() const
        {
            return GetAddress();
        }

        std::uintptr_t GetAddress() const
        {
            return _address;
        }
    };
}  // namespace IWXMVM::Signatures
