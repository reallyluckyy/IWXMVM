#pragma once

namespace IWXMVM::Components
{
    namespace Playback
    {
        static constexpr std::array TIMESCALE_STEPS{
            0.001f, 0.005f, 0.01f,  0.025f, 0.05f, 0.1f,  0.125f, 0.2f,
            0.25f, 0.333f, 0.5f, 0.75f, 1.0f, 1.25f, 1.5f,
            2.0f, 5.0f, 10.0f, 20.0f, 50.0f
        };

        void TogglePaused();
        bool IsPaused();

        void SkipForward(std::int32_t ticks);
        void SetTickDelta(std::int32_t value, bool ignoreDeadzone = false);

        std::int32_t CalculatePlaybackDelta(std::int32_t gameMsec);
    } // namespace Playback
}  // namespace IWXMVM::Components