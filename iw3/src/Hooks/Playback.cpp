#include "StdInclude.hpp"
#include "Playback.hpp"

#include "Utilities/HookManager.hpp"
#include "Mod.hpp"


namespace IWXMVM::IW3::Hooks::Playback 
{
#undef min
#undef max

	void GeneratePattern(auto& pattern, float fps, float timescale)
	{
		// SV_Frame is called once per frame, so we calculate the number of calls it takes to advance 1000 ms
		// by using the following formula: fps / timescale

		// example: 500 fps, 0.01 timescale
		// 500 / 0.01 = 50'000
		// we need to generate a pattern where 1000 * 1ms and 49000 * 0ms are interleaved, and fit this into an array of size 1000

		const std::size_t callCountPerSecond = static_cast<std::size_t>(std::clamp(fps / timescale, 1000.0f, 1'000'000.0f));

		const std::size_t onesCount = 1000 / (callCountPerSecond / 1000.0);
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

	void SV_Frame_Internal(std::int32_t& msec)
	{
		// always return 0 msec when pausing demo playback
		if (Mod::GetGameInterface()->IsDemoPlaybackPaused()) 
		{
			msec = 0;
			return;
		}

		const std::optional<Types::Dvar> timescale = Mod::GetGameInterface()->GetDvar("timescale");

		// we can use the original msec value when its value is greater than 1, and/or when timescale is equal or greater than 1.0
		if (msec > 1 || !timescale.has_value() || timescale.value().value->floating_point >= 1.0f)
			return;

		const std::optional<Types::Dvar> com_maxfps = Mod::GetGameInterface()->GetDvar("com_maxfps");
		if (!com_maxfps.has_value())
			return;

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

			if (lastMaxFps > com_maxfps.value().value->int32)  // max fps was decreased, imgui fps is potentially too high
				frameRate = std::min(ImGui::GetIO().Framerate, static_cast<float>(com_maxfps.value().value->int32));
			else if (lastMaxFps < com_maxfps.value().value->int32) // max fps was increased, imgui fps is potentially too low
				frameRate = std::max(ImGui::GetIO().Framerate, static_cast<float>(com_maxfps.value().value->int32));
			else 
			{
				assert(lastTimeScale != timescale.value().value->floating_point);
				frameRate = static_cast<float>(com_maxfps.value().value->int32);
			}

			lastTimeScale = timescale.value().value->floating_point;
			lastMaxFps = com_maxfps.value().value->int32;
			patternIndex = 0;

			GeneratePattern(pattern, frameRate, timescale.value().value->floating_point);
		} 
		else if (patternIndex % 1000 == 0)
			GeneratePattern(pattern, ImGui::GetIO().Framerate, timescale.value().value->floating_point);

		// advance (1ms) or pause(0ms) based on the pattern
		msec = pattern[patternIndex++ % 1000];
	}

	void __declspec(naked) SV_Frame_Hook()
	{
		static std::int32_t msec;
		static std::int32_t ebxValue;

		__asm
		{
			pop ecx
			pushad
			mov msec, esi
			mov ebxValue, ebx
		}

		if (ebxValue == *reinterpret_cast<std::int32_t*>(0x69136C)) // not true for CoD4X
		{ 
			SV_Frame_Internal(msec);
			__asm
			{
				popad
				mov eax, msec
				ret
			}
		} 
		else 
		{
			SV_Frame_Internal(msec);
			__asm
			{
				popad
				mov ebx, msec
				ret
			}
		}
	}

	void Install() 
	{
		HookManager::CreateHook(0x53366E, (std::uintptr_t)SV_Frame_Hook, nullptr);
	}
}
