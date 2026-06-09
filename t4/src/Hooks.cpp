#include "StdInclude.hpp"
#include "Hooks.hpp"

#include "Hooks/Playback.hpp"
#include "Hooks/Camera.hpp"
#include "Hooks/Rendering.hpp"
#include "Hooks/PlayerAnimation.hpp"
#include "Hooks/HUD.hpp"

namespace IWXMVM::T4::Hooks
{
    void Install()
    {
        Hooks::Playback::Install();
        Hooks::Camera::Install();
        Hooks::PlayerAnimation::Install();
        Hooks::Rendering::Install();
        Hooks::HUD::Install();
    }
}  // namespace IWXMVM::T4::Hooks
