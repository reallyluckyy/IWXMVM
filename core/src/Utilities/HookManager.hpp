#pragma once

namespace IWXMVM::HookManager
{
    // TODO: Store placed hooks and unhook in Unhook function

    bool WriteJump(std::uintptr_t from, std::uintptr_t to);
    bool WriteCall(std::uintptr_t from, std::uintptr_t to);

    void CreateHook(std::uintptr_t originalPtr, std::uintptr_t detourPtr, std::uintptr_t* trampolinePtr);
    void Unhook();
};  // namespace IWXMVM::HookManager