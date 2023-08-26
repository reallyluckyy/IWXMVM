#pragma once

namespace IWXMVM::MathUtils
{
	float DegreesToRadians(float degrees);
	float RadiansToDegrees(float radians);

	glm::vec3 ForwardVectorFromAngles(float pitch, float yaw, float roll);
	glm::vec3 AnglesFromForwardVector(glm::vec3 forward);
}