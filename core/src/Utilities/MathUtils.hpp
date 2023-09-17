#pragma once

#include "Components/Camera.hpp"

namespace IWXMVM::MathUtils
{
	glm::vec3 ForwardVectorFromAngles(float pitch, float yaw, float roll);
	glm::vec3 AnglesFromForwardVector(glm::vec3 forward);

	std::optional<ImVec2> WorldToScreenPoint(glm::vec3 point, Components::Camera& camera, glm::vec4 viewport);
}