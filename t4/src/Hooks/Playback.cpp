#include "StdInclude.hpp"
#include "Playback.hpp"

#include "Components/Playback.hpp"
#include "Components/Rewinding.hpp"
#include "Utilities/HookManager.hpp"
#include "Events.hpp"
#include "../Addresses.hpp"
#include "../Structures.hpp"
#include "../Functions.hpp"
#include "../Patches.hpp"

namespace IWXMVM::T4::Hooks::Playback
{
    void SV_Frame_Internal(std::int32_t& msec)
    {
        msec = Components::Playback::CalculatePlaybackDelta(msec);
    }

    void __declspec(naked) SV_Frame_Hook()
    {
        static std::int32_t msec;

        __asm
        {
            pop ecx
            pushad
            mov msec, esi
        }

        SV_Frame_Internal(msec);
        __asm
        {
            popad
            mov eax, msec
            ret
        }
    }

    typedef int (*FS_Read_t)(void* buffer, int len, int f);
    FS_Read_t FS_Read_Trampoline;
    int FS_Read_Hook(void* buffer, int len, int f)
    {
        using namespace Structures;
        fileHandleData_t fh =
            *reinterpret_cast<fileHandleData_t*>(GetGameAddresses().fsh() + f * sizeof(fileHandleData_t));

        std::string_view sv = fh.name;
        bool isDemoFile = sv.ends_with(Mod::GetGameInterface()->GetDemoExtension());

        if (!isDemoFile)
        {
            return FS_Read_Trampoline(buffer, len, f);
        }

        auto result = Components::Rewinding::FS_Read(buffer, len);
        if (result == -1)
        {
            return FS_Read_Trampoline(buffer, len, f);
        }

        return result;
    }

    typedef void (*CL_PlayDemo_f_t)();
    CL_PlayDemo_f_t CL_PlayDemo_f_Trampoline;
    void CL_PlayDemo_f_Hook() 
    {
        CL_PlayDemo_f_Trampoline();
        Events::Invoke(EventType::PostDemoLoad);
    }

    LONG __stdcall ExceptionHandler_Hook(struct _EXCEPTION_POINTERS* ExceptionInfo)
    {
        LOG_INFO("Exception at address: {}", ExceptionInfo->ExceptionRecord->ExceptionAddress);
        return 0xFFFFFFF;
    }

    void Install()
    {
        HookManager::CreateHook(0x5D0460, (std::uintptr_t)ExceptionHandler_Hook, nullptr);

        HookManager::CreateHook(GetGameAddresses().CL_PlayDemo_f(), (std::uintptr_t)CL_PlayDemo_f_Hook,
                               (uintptr_t*)&CL_PlayDemo_f_Trampoline);

        HookManager::CreateHook(GetGameAddresses().SV_Frame(), (std::uintptr_t)SV_Frame_Hook, nullptr);

        HookManager::CreateHook(GetGameAddresses().FS_Read(), (std::uintptr_t)FS_Read_Hook, (uintptr_t*)&FS_Read_Trampoline);
    }
}  // namespace IWXMVM::T4::Hooks::Playback
