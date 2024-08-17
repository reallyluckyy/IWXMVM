#include "StdInclude.hpp"
#include "Hooks.hpp"

#include "Utilities/HookManager.hpp"
#include "Addresses.hpp"
#include "Events.hpp"

#include "Hooks/Playback.hpp"
#include "Hooks/Camera.hpp"

namespace IWXMVM::IW5::Hooks
{
    typedef void (*CL_Demo_Play_t)();
    CL_Demo_Play_t CL_Demo_Play_Trampoline = nullptr;

    void CL_Demo_Play_Hook()
    {
        CL_Demo_Play_Trampoline();

        Events::Invoke(EventType::PostDemoLoad);
        Events::Invoke(EventType::OnDemoBoundsDetermined);
    }

    typedef char (__cdecl * CG_VisionSetStartLerp_To_t)(int cvsData, int mode, int style, char* nameTo, int duration,
                                              int clientTime);
    CG_VisionSetStartLerp_To_t CG_VisionSetStartLerp_To_Trampoline;
    char CG_VisionSetStartLerp_To_Hook(int cvsData, int mode, int style, char* nameTo, int duration,
                                             int clientTime)
    {
        // Dont transition to the intro vision, which causes the black/white view in the beginning of a match
        // That way we dont get any issues when rewinding to the beginning of a demo
        if (std::string_view(nameTo).compare("mpIntro") == 0 || std::string_view(nameTo).compare("mpintro") == 0)
		{
            return 1;
		}
        return CG_VisionSetStartLerp_To_Trampoline(cvsData, mode, style, nameTo, duration, clientTime);
    }


    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().CL_Demo_Play(), (std::uintptr_t)CL_Demo_Play_Hook,
                                (uintptr_t*)&CL_Demo_Play_Trampoline);

        HookManager::CreateHook(GetGameAddresses().CG_VisionSetStartLerp_To(), (std::uintptr_t)CG_VisionSetStartLerp_To_Hook,
                                (uintptr_t*)&CG_VisionSetStartLerp_To_Trampoline);

        Hooks::Playback::Install();
        Hooks::Camera::Install();
    }
}  // namespace IWXMVM::IW5::Hooks
