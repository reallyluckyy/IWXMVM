#pragma once

namespace ImGui
{
	bool DemoProgressBar(uint32_t* currentTick, const uint32_t endTick, const ImVec2& size_arg, const char* overlay);
	bool SliderFloatSteps(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags);
	bool SliderScalarSteps(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags);
}