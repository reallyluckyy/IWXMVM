#pragma once

namespace ImGui
{
	void DemoProgressBarLines(std::uint32_t currentTick, std::uint32_t endTick);
	bool TimescaleSlider(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags);
}
