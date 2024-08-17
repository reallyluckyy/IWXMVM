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

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().CL_Demo_Play(), (std::uintptr_t)CL_Demo_Play_Hook,
                                (uintptr_t*)&CL_Demo_Play_Trampoline);

        Hooks::Playback::Install();
        Hooks::Camera::Install();
    }
}  // namespace IWXMVM::IW5::Hooks
