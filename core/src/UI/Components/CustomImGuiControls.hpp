#pragma once

namespace ImGui
{
	bool DemoProgressBar(uint32_t* currentTick, const uint32_t endTick, const ImVec2& size_arg, const char* overlay);
	bool TimescaleSlider(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags);
}