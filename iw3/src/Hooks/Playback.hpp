#pragma once

namespace IWXMVM::IW3::Hooks::Playback
{
    void SkipForward(std::int32_t ticks);
    void RewindBy(std::int32_t ticks);
    void Install();

    void Reset();

    extern std::atomic<std::int32_t> rewindTo;
}