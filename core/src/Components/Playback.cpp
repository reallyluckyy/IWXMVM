#include "StdInclude.hpp"
#include "Playback.hpp"

#include "Mod.hpp"
#include "Rewinding.hpp"

namespace IWXMVM::Components::Playback
{
    bool isPlaybackPaused = false;
    uint32_t timelineTick = 0;
    std::optional<uint32_t> frozenTick = std::nullopt;

    void TogglePaused()
    {
        isPlaybackPaused = !isPlaybackPaused;
    }

    bool IsPaused()
    {
        return isPlaybackPaused;
    }

    uint32_t GetTimelineTick()
    {
        if (!frozenTick.has_value())
        {
            timelineTick = Mod::GetGameInterface()->GetDemoInfo().gameTick;
        }

        return timelineTick;
	}

    void SetTimelineTick(uint32_t tick)
    {
        timelineTick = tick;
    }

    std::optional<uint32_t> GetFrozenTick()
    {
        return frozenTick;
    }

    bool IsGameFrozen()
    {
        return frozenTick.has_value();
    }

    void ToggleFrozenTick()
    {
        if (frozenTick.has_value())
        {
            frozenTick.reset();
        }
        else
        {
            frozenTick.emplace(GetTimelineTick());
        }
    }
    
    void SkipForward(std::int32_t ticks)
    {
        if (frozenTick.has_value())
        {
            timelineTick += ticks;
            return;
        }

        SkipDemoForward(ticks);
    }

    void SkipDemoForward(std::int32_t ticks)
    {
        auto addresses = Mod::GetGameInterface()->GetPlaybackDataAddresses();
        auto realtime = reinterpret_cast<int32_t*>(addresses.cls.realtime);
        *realtime = *realtime + ticks;
        LOG_DEBUG("Skipping forward {} ticks, realtime: {}", ticks, *realtime);
    }

    void SetTickDelta(int32_t value, bool ignoreDeadzone)
    {
        if (value > 0)
            SkipForward(value);
        else if ((value < -REWIND_DEADZONE) || (value < 0 && ignoreDeadzone))
            Rewinding::RewindBy(value);
    }

    void HandleImportedFrozenTickLogic(std::optional<std::uint32_t> importedFrozenTick)
    {
        const auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
        
        if (frozenTick.has_value())
        {
            // toggle frozen tick off so we can rewind the actual state of the game
            frozenTick.reset();
        }

        if (!importedFrozenTick.has_value())
        {
            // since no frozen tick is set in the imported file, we're done here
            return;
        }
        
        // set actual tick
        Playback::SetTickDelta(importedFrozenTick.value() - demoInfo.gameTick, true);
        
        // (re)enable frozen tick with specified value
        frozenTick.emplace(importedFrozenTick.value());
        
        // update current (frozen) tick
        SetTimelineTick(importedFrozenTick.value());
        
        if (!Playback::IsPaused())
        {
            Playback::TogglePaused();
        }
    }

    void GeneratePattern(auto& pattern, float fps, float timescale)
    {
        // SV_Frame is called once per frame, so we calculate the number of calls it takes to advance 1000 ms
        // by using the following formula: fps / timescale

        // example: 500 fps, 0.01 timescale
        // 500 / 0.01 = 50'000
        // we need to generate a pattern where 1000 * 1ms and 49000 * 0ms are interleaved, and fit this into an array of
        // size 1000

        const std::size_t callCountPerSecond =
            static_cast<std::size_t>(std::clamp(fps / timescale, 1000.0f, 1'000'000.0f));

        const std::size_t onesCount = static_cast<size_t>(pattern.size() / (callCountPerSecond / static_cast<float>(pattern.size())));
        const std::size_t zerosCount = pattern.size() - onesCount;

        for (std::size_t i = 0, zeros = 1, ones = 1; i < pattern.size(); ++i)
        {
            if (ones * zerosCount < zeros * onesCount)
            {
                ++ones;
                pattern[i] = 1;
            }
            else
            {
                ++zeros;
                pattern[i] = 0;
            }
        }

        assert(std::accumulate(pattern.begin(), pattern.end(), 0) > 0);
    }

    std::int32_t CalculatePlaybackDeltaInternal(std::int32_t gameMsec)
    {
        // check if we need to skip forward for exact rewinding
        if (Components::Rewinding::CheckSkipForward())
        {
            return 0;
        }

        auto& captureManager = Components::CaptureManager::Get();
        if (captureManager.IsCapturing())
        {
            return captureManager.OnGameFrame();
        }

        // workaround for low timescales that would take a couple of seconds to trigger the rewind process to begin
        if (Components::Rewinding::IsRewinding())
        {
            return 50;
        }

        // always return 0 msec when pausing demo playback
        if (IsPaused())
        {
            return 0;
        }

        const std::optional<Types::Dvar> timescale = Mod::GetGameInterface()->GetDvar("timescale");

        // we can use the original msec value when its value is greater than 1, and/or when timescale is equal or
        // greater than 1.0
        if (gameMsec > 1 || !timescale.has_value() || timescale.value().value->floating_point >= 1.0f)
            return gameMsec;

        const std::optional<Types::Dvar> com_maxfps = Mod::GetGameInterface()->GetDvar("com_maxfps");
        if (!com_maxfps.has_value())
            return gameMsec;

        static float lastTimeScale = timescale.value().value->floating_point;
        static std::int32_t lastMaxFps = com_maxfps.value().value->int32;
        static std::array<std::uint8_t, 1000> pattern{};
        static std::size_t patternIndex = 0;

        // below we're going to generate a pattern of interleaved 0s and 1s based on (imgui) frame times
        // we generate a new pattern each second, or whenever timescale or com_maxfps values changes

        if (lastTimeScale != timescale.value().value->floating_point || lastMaxFps != com_maxfps.value().value->int32)
        {
            // this branch ensures that any change to the timescale or max fps immediately changes the pattern
            float frameRate;

            if (lastMaxFps >
                com_maxfps.value().value->int32)  // max fps was decreased, imgui fps is potentially too high
                frameRate = std::min(ImGui::GetIO().Framerate, static_cast<float>(com_maxfps.value().value->int32));
            else if (lastMaxFps <
                     com_maxfps.value().value->int32)  // max fps was increased, imgui fps is potentially too low
                frameRate = std::max(ImGui::GetIO().Framerate, static_cast<float>(com_maxfps.value().value->int32));
            else
            {
                assert(lastTimeScale != timescale.value().value->floating_point);
                frameRate = ImGui::GetIO().Framerate;
            }

            lastTimeScale = timescale.value().value->floating_point;
            lastMaxFps = com_maxfps.value().value->int32;
            patternIndex = 0;

            GeneratePattern(pattern, frameRate, timescale.value().value->floating_point);
        }
        else if (patternIndex % 1000 == 0)
            GeneratePattern(pattern, ImGui::GetIO().Framerate, timescale.value().value->floating_point);

        // advance (1ms) or pause(0ms) based on the pattern
        return pattern[patternIndex++ % 1000];
    }

    std::int32_t CalculatePlaybackDelta(std::int32_t gameMsec)
    {
        auto delta = CalculatePlaybackDeltaInternal(gameMsec);

        if (IsGameFrozen())
        {
            SetTimelineTick(GetTimelineTick() + delta);
            return 0;
        }

        return delta;
    }
}  // namespace IWXMVM::Components::Playback
