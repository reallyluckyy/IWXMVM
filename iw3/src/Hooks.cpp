#include "StdInclude.hpp"
#include "Hooks.hpp"

#include "Hooks/Commands.hpp"
#include "Hooks/Playback.hpp"
#include "Hooks/Camera.hpp"
#include "Hooks/Rendering.hpp"
#include "Hooks/PlayerAnimation.hpp"

namespace IWXMVM::IW3::Hooks
{
    void Install()
    {
        Hooks::Playback::Install();
        Hooks::Commands::Install();
        Hooks::Camera::Install();
        Hooks::PlayerAnimation::Install();
        Hooks::Rendering::Install();
    }
}  // namespace IWXMVM::IW3::Hooks
