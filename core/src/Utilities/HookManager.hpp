#pragma once

namespace IWXMVM::HookManager
{
    bool WriteJump(std::uintptr_t from, std::uintptr_t to);
    bool WriteCall(std::uintptr_t from, std::uintptr_t to);

    void CreateHook(std::uintptr_t originalPtr, std::uintptr_t detourPtr, std::uintptr_t* trampolinePtr);
    void Unhook(std::uintptr_t originalPtr);
    void UnhookAll();
};  // namespace IWXMVM::HookManager