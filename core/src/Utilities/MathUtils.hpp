#pragma once
#include "Types/Vector3.hpp"

namespace IWXMVM::MathUtils
{
	float DegreesToRadians(float degrees);
	float RadiansToDegrees(float radians);

	Vector3 ForwardVectorFromAngles(float pitch, float yaw, float roll);
	Vector3 AnglesFromForwardVector(Vector3 forward);
}