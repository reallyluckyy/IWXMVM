#pragma once

namespace IWXMVM::Components
{
    namespace Playback
    {
        void TogglePaused();
        bool IsPaused();
        std::int32_t CalculatePlaybackDelta(std::int32_t gameMsec);
    } // namespace Playback
}  // namespace IWXMVM::Components