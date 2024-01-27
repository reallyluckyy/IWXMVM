#pragma once

#include "Components/Camera.hpp"
#include "Types/Keyframe.hpp"

namespace IWXMVM::MathUtils
{
    glm::vec3 ForwardVectorFromAngles(glm::vec3 eulerAngles);
    glm::vec3 AnglesFromForwardVector(glm::vec3 forward);
    glm::vec3 AnglesFromAxis(glm::vec3 forward, glm::vec3 right, glm::vec3 up);

    std::optional<ImVec2> WorldToScreenPoint(glm::vec3 point, Components::Camera& camera);
    float InterpolateCubicSpline(const std::vector<Types::Keyframe>& keyframes, uint32_t valueIndex, float tick);
}  // namespace IWXMVM::MathUtils