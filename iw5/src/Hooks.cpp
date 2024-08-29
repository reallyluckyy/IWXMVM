#include "StdInclude.hpp"
#include "Hooks.hpp"

#include "Hooks/Playback.hpp"
#include "Hooks/Camera.hpp"

namespace IWXMVM::IW5::Hooks
{
    void Install()
    {
        Hooks::Playback::Install();
        Hooks::Camera::Install();
    }
}  // namespace IWXMVM::IW5::Hooks
