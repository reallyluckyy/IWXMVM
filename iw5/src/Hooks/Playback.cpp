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

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().Com_TimeScaleMsec(), (std::uintptr_t)Com_TimeScaleMsec_Hook,
                                (uintptr_t *)&Com_TimeScaleMsec_Trampoline);
    }
}  // namespace IWXMVM::IW5::Hooks::Playback
