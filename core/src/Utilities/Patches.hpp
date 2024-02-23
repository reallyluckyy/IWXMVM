#pragma once
#include "StdInclude.hpp"

namespace IWXMVM::Patches
{
    inline constexpr std::uint8_t HexToDecimal(char c)
    {
        constexpr std::array<std::uint8_t, 4> boundaries{'0', '9', 'A', 'F'};
        const std::uint8_t val = static_cast<std::uint8_t>(c);

        if (val >= boundaries[0] && val <= boundaries[1])  // 0-9
            return val - boundaries[0];

        if (val >= boundaries[2] && val <= boundaries[3])  // A-F
            return val - boundaries[2] + 10;

        throw std::runtime_error("Not a hexadecimal value.");
    }

    template <std::size_t length>
    struct ByteString
    {
        static constexpr std::size_t byteCount = (length + 1) / 3;
        std::array<std::uint8_t, byteCount> bytes{};

        constexpr ByteString(const char (&str)[length])
        {
            std::size_t spaceCount = 0;

            for (std::size_t strIndex = 0, byteIndex = 0; strIndex + 1 < length;)
            {
                if (str[strIndex] == ' ')
                {
                    ++spaceCount;
                    ++strIndex;
                    continue;
                }

                bytes[byteIndex++] = (HexToDecimal(str[strIndex]) << 4) + HexToDecimal(str[strIndex + 1]);
                strIndex += 2;
            }

            if (spaceCount != (length - 1) / 3 && spaceCount != 1 + (length - 1) / 3)
                throw std::runtime_error("Incorrect signature input.");
        };
    };

    template <ByteString byteString>
    constexpr auto operator"" _bytes()
    {
        return byteString.bytes;
    }

    enum struct PatchApplySetting
    {
        Deferred,
        Immediately
    };

    template <std::size_t length>
    class Patch
    {
       public:
        Patch(std::uintptr_t dst, std::array<std::uint8_t, length> bytes, PatchApplySetting setting = {})
            : _dst(reinterpret_cast<std::uint8_t*>(dst)), _src(bytes)
        {
            assert(_dst != nullptr);

            if (setting != PatchApplySetting::Deferred)
                Apply();
        }

        ~Patch()
        {
            Revert();
        }

        Patch(const Patch& other) = delete;
        Patch(Patch&& other) noexcept = delete;
        Patch& operator=(const Patch& other) = delete;
        Patch& operator=(Patch&& other) noexcept = delete;

        void Apply()
        {
            if (!_active)
            {
                DWORD oldProtection;
                ::VirtualProtect(_dst, _size, PAGE_EXECUTE_READWRITE, &oldProtection);

                std::memcpy(_originalBytes.data(), _dst, _size);
                std::memcpy(_dst, _src.data(), _size);

                ::VirtualProtect(_dst, _size, oldProtection, &oldProtection);
                _active = true;
            }
        }

        void Revert()
        {
            if (_active)
            {
                DWORD oldProtection;
                ::VirtualProtect(_dst, _size, PAGE_EXECUTE_READWRITE, &oldProtection);

                std::memcpy(_dst, _originalBytes.data(), _size);

                ::VirtualProtect(_dst, _size, oldProtection, &oldProtection);
                _active = false;
            }
        }

        bool IsApplied()
        {
            return _active;
        }

       private:
        static constexpr std::size_t _size = length;

        bool _active{};
        std::uint8_t* _dst{};
        std::array<std::uint8_t, _size> _src{};
        std::array<std::uint8_t, _size> _originalBytes{};
    };

    class ReturnPatch : public Patch<1>
    {
       public:
        ReturnPatch(std::uintptr_t dst, PatchApplySetting setting = {})
            : Patch<1>(dst, std::array{RETURN_OPCODE}, setting)
        {
        }

       private:
        static constexpr std::uint8_t RETURN_OPCODE = 0xC3;
    };

    class JumpPatch : public Patch<1>
    {
       public:
        JumpPatch(std::uintptr_t dst, PatchApplySetting setting = {})
            : Patch<1>(dst, std::array{UNCOND_REL_JUMP_OPCODE}, setting)
        {
        }

       private:
        static constexpr std::uint8_t UNCOND_REL_JUMP_OPCODE = 0xEB;
    };

    template <std::size_t count>
    class NopPatch : public Patch<count>
    {
       public:
        NopPatch(std::uintptr_t dst, PatchApplySetting setting = {}) : Patch<count>(dst, _bytes, setting)
        {
        }

       private:
        static constexpr std::uint8_t NOP_OPCODE = 0x90;

        static constexpr auto _bytes = []() {
            std::array<std::uint8_t, count> tmp;
            tmp.fill(NOP_OPCODE);

            return tmp;
        }();
    };
}  // namespace IWXMVM::Patches