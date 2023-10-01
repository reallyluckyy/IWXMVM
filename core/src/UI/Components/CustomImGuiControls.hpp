#pragma once

#include "Types/CampathNode.hpp"

namespace ImGui
{
	void TimelineMarkers(const std::vector<IWXMVM::Types::CampathNode>& nodes, std::uint32_t endTick);
	void DemoProgressBarLines(std::uint32_t currentTick, std::uint32_t endTick);
	bool TimescaleSlider(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags);
}
