module;

#include "StdInclude.hpp"
#include "MinHook.h"
#include "UI/UIManager.hpp"

export module HookManager;

namespace IWXMVM::HookManager
{
    constexpr std::uint8_t JUMP_OPCODE = 0xE9;
    constexpr std::uint8_t CALL_OPCODE = 0xE8;
    constexpr std::size_t JUMP_LENGTH = 5;

    bool WriteJumpInternal(std::uintptr_t from, std::uintptr_t to, std::size_t size = JUMP_LENGTH,
                           std::uint8_t jumpOpcode = JUMP_OPCODE)
    {
        DWORD curProtection;

        if (!VirtualProtect(reinterpret_cast<LPVOID>(from), size, PAGE_EXECUTE_READWRITE, &curProtection))
        {
            throw std::runtime_error("VirtualProtect failure. Address: " + std::to_string(from) +
                                     ", size: " + std::to_string(size));
            return false;
        }

        std::uintptr_t relativeAddress = (to - from) - 5;
        *reinterpret_cast<uint8_t*>(from) = jumpOpcode;
        *reinterpret_cast<std::uintptr_t*>(from + 1) = relativeAddress;

        if (*reinterpret_cast<uint8_t*>(from) != jumpOpcode ||
            *reinterpret_cast<std::uintptr_t*>(from + 1) != relativeAddress)
        {
            throw std::runtime_error("Memory access failure. Addresses: " + std::to_string(from) + ", " +
                                     std::to_string(from + 1));
            return false;
        }

        if (!VirtualProtect(reinterpret_cast<LPVOID>(from), size, curProtection, &curProtection))
        {
            throw std::runtime_error("VirtualProtect failure. Address: " + std::to_string(from) +
                                     ", size: " + std::to_string(size));
            return false;
        }

        return true;
    }

    export bool WriteJump(std::uintptr_t from, std::uintptr_t to)
    {
        return WriteJumpInternal(from, to, JUMP_LENGTH, JUMP_OPCODE);
    }

    export bool WriteCall(std::uintptr_t from, std::uintptr_t to)
    {
        return WriteJumpInternal(from, to, JUMP_LENGTH, CALL_OPCODE);
    }

    export void CreateHook(std::uintptr_t originalPtr, std::uintptr_t detourPtr, std::uintptr_t* trampolinePtr)
    {
        auto result = MH_CreateHook((void*)originalPtr, (void*)detourPtr, (void**)trampolinePtr);
        if (result != MH_OK)
        {
            throw std::runtime_error(
                std::format("Failed to create hook for address {:X}: {}", originalPtr, magic_enum::enum_name(result)).c_str()
            );
        }

        result = MH_EnableHook((void*)originalPtr);
        if (result != MH_OK)
        {
            throw std::runtime_error(
                std::format("Failed to enable hook for address {:X}: {}", originalPtr, magic_enum::enum_name(result))
                    .c_str());
        }
    }

    export void Unhook()
    {
        if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK)
            throw std::runtime_error("Failed to disable hooks");
        if (MH_Uninitialize() != MH_OK)
            throw std::runtime_error("Failed to uninitialize MinHook");
    }
}  // namespace IWXMVM::HookManager