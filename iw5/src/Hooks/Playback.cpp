#include "StdInclude.hpp"
#include "Playback.hpp"

#include "Components/Playback.hpp"
#include "Utilities/HookManager.hpp"
#include "../Addresses.hpp"

namespace IWXMVM::IW5::Hooks::Playback
{
    typedef int32_t(__cdecl *Com_TimeScaleMsec_t)(int32_t msec);
    Com_TimeScaleMsec_t Com_TimeScaleMsec_Trampoline = nullptr;

    int32_t Com_TimeScaleMsec_Hook(std::int32_t msec)
    {
        // TODO: this causes the framerate to drop when on low timescales,
        // so something isnt right here yet
        msec = Com_TimeScaleMsec_Trampoline(msec);
        return Components::Playback::CalculatePlaybackDelta(msec);
    }

    typedef int (*CL_Demo_ReadInternal_t)(int handle, void* buffer, int len);
    CL_Demo_ReadInternal_t CL_Demo_ReadInternal_Trampoline;
    int CL_Demo_ReadInternal_Hook(int handle, void* buffer, int len)
    {
        bool isDemoFile = handle == *(int*)0xB7F984; // clc_clientDemo_demoFileHandle

        if (!isDemoFile)
        {
            return CL_Demo_ReadInternal_Trampoline(handle, buffer, len);
        }

        auto result = Components::Rewinding::FS_Read(buffer, len);
        if (result == -1)
        {
            return CL_Demo_ReadInternal_Trampoline(handle, buffer, len);
        }
        else
        {
            *(DWORD*)0x65FBBC8 += len;
        }

        return result;
    }

    typedef int (*CL_Demo_SeekFile_t)(int, int, int);
    CL_Demo_SeekFile_t CL_Demo_SeekFile_Trampoline;
    int CL_Demo_SeekFile_Hook(int handle, int offset, int origin)
    {
        LOG_DEBUG("Seeking to offset: {} with origin: {}", offset, origin);
        
        bool isDemoFile = handle == *(int*)0xB7F984;  // clc_clientDemo_demoFileHandle

        if (!isDemoFile)
        {
            return CL_Demo_SeekFile_Trampoline(handle, offset, origin);
        }

        auto result = Components::Rewinding::FS_Seek(offset, origin);
        if (result == -1)
        {
            return CL_Demo_SeekFile_Trampoline(handle, offset, origin);
        }

        return result;
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().Com_TimeScaleMsec(), (std::uintptr_t)Com_TimeScaleMsec_Hook,
                                (uintptr_t *)&Com_TimeScaleMsec_Trampoline);

        HookManager::CreateHook(GetGameAddresses().CL_Demo_ReadInternal(), (std::uintptr_t)CL_Demo_ReadInternal_Hook,
                                (uintptr_t*)&CL_Demo_ReadInternal_Trampoline);

        HookManager::CreateHook(0x6A6130, (std::uintptr_t)CL_Demo_SeekFile_Hook, (uintptr_t*)&CL_Demo_SeekFile_Trampoline);
    }
}  // namespace IWXMVM::IW5::Hooks::Playback
