#pragma once

#include "Types/CampathNode.hpp"
#include "Types/KeyframeableProperty.hpp"
#include "Types/Keyframe.hpp"
#include "Components/Camera.hpp"

namespace ImGuiEx
{
    void DrawLine3D(glm::vec3 from, glm::vec3 to, ImVec4 color, float thickness);
    void DrawPoint3D(glm::vec3 point, ImVec4 color, ImVec2 size = ImVec2(3, 3));

    void DemoProgressBarLines(std::uint32_t currentTick, std::uint32_t endTick);
    bool TimescaleSlider(const char* label, float* v, float v_min, float v_max, const char* format,
                         ImGuiSliderFlags flags);
    void DrawKeyframeSliderInternal(const IWXMVM::Types::KeyframeableProperty& property, uint32_t* currentTick, uint32_t* startTick,
                                    uint32_t* endTick, std::vector<IWXMVM::Types::Keyframe>& keyframes);
    void DrawCurveEditorInternal(const IWXMVM::Types::KeyframeableProperty& property, uint32_t* currentTick,
                                    uint32_t* startTick, uint32_t* endTick,
                                    std::vector<IWXMVM::Types::Keyframe>& keyframes);
}  // namespace ImGuiEx
