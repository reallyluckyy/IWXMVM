#include "StdInclude.hpp"
#include "Playback.hpp"

#include "../Structures.hpp"
#include "../Addresses.hpp"

#include "Components/Playback.hpp"
#include "Utilities/HookManager.hpp"
#include "Events.hpp"

namespace IWXMVM::IW5::Hooks::Playback
{

    typedef void (*CL_Demo_Play_t)();
    CL_Demo_Play_t CL_Demo_Play_Trampoline = nullptr;
    void CL_Demo_Play_Hook()
    {
        CL_Demo_Play_Trampoline();

        Events::Invoke(EventType::PostDemoLoad);
        Events::Invoke(EventType::OnDemoBoundsDetermined);
    }


    typedef int32_t(__cdecl *Com_TimeScaleMsec_t)(int32_t msec);
    Com_TimeScaleMsec_t Com_TimeScaleMsec_Trampoline = nullptr;
    int32_t Com_TimeScaleMsec_Hook(std::int32_t msec)
    {
        // TODO: this causes the framerate to drop when on low timescales,
        // so something isnt right here yet
        msec = Com_TimeScaleMsec_Trampoline(msec);
        return Components::Playback::CalculatePlaybackDelta(msec);
    }

    typedef int (*CL_Demo_ReadInternal_t)(void* handle, void* buffer, int len);
    CL_Demo_ReadInternal_t CL_Demo_ReadInternal_Trampoline;
    int CL_Demo_ReadInternal_Hook(void* handle, void* buffer, int len)
    {
        const auto clc = Structures::GetClientConnection();
        if (handle != clc->clientDemo.demoFileHandle)
        {
            return CL_Demo_ReadInternal_Trampoline(handle, buffer, len);
        }

        auto result = Components::Rewinding::FS_Read(buffer, len);
        if (result == -1)
        {
            return CL_Demo_ReadInternal_Trampoline(handle, buffer, len);
        }

        return result;
    }

    typedef int (*CL_Demo_SeekFile_t)(void*, int, int);
    CL_Demo_SeekFile_t CL_Demo_SeekFile_Trampoline;
    int CL_Demo_SeekFile_Hook(void* handle, int offset, int origin)
    {
        const auto clc = Structures::GetClientConnection();
        if (handle != clc->clientDemo.demoFileHandle)
        {
            return CL_Demo_SeekFile_Trampoline(handle, offset, origin);
        }

        auto convertedOrigin = (origin + 1) % 3;

        auto result = Components::Rewinding::FS_Seek(offset, convertedOrigin);
        if (result == -1)
        {
            return CL_Demo_SeekFile_Trampoline(handle, offset, origin);
        }

        return result;
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().CL_Demo_Play(), (std::uintptr_t)CL_Demo_Play_Hook,
                                (uintptr_t*)&CL_Demo_Play_Trampoline);

        HookManager::CreateHook(GetGameAddresses().Com_TimeScaleMsec(), (std::uintptr_t)Com_TimeScaleMsec_Hook,
                                (uintptr_t *)&Com_TimeScaleMsec_Trampoline);

        HookManager::CreateHook(GetGameAddresses().CL_Demo_ReadInternal(), (std::uintptr_t)CL_Demo_ReadInternal_Hook,
                                (uintptr_t*)&CL_Demo_ReadInternal_Trampoline);

        HookManager::CreateHook(GetGameAddresses().CL_Demo_SeekFile(), (std::uintptr_t)CL_Demo_SeekFile_Hook,
                                (uintptr_t*)&CL_Demo_SeekFile_Trampoline);
    }
}  // namespace IWXMVM::IW5::Hooks::Playback
