#pragma once

#include "imgui_internal.h"

namespace ImGuiEx
{
    void DrawLine3D(glm::vec3 from, glm::vec3 to, ImVec4 color, float thickness);
    void DrawPoint3D(glm::vec3 point, ImVec4 color, ImVec2 size = ImVec2(3, 3));

    bool TimescaleSlider(const char* label, float* v, float v_min, float v_max, const char* format,
                         ImGuiSliderFlags flags);
    void DemoProgressBarLines(ImRect rect, std::uint32_t currentTick, std::uint32_t displayStartTick,
                              std::uint32_t displayEndTick, std::uint32_t demoLength);
}  // namespace ImGuiEx
