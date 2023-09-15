#pragma once

#include "Types/Marker.hpp"

namespace ImGui
{
	void TimelineMarkers(const std::vector<IWXMVM::Types::Marker>& markers, std::uint32_t endTick);
	void DemoProgressBarLines(std::uint32_t currentTick, std::uint32_t endTick);
	bool TimescaleSlider(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags);
}
