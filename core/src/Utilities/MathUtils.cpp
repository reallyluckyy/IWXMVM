#include "StdInclude.hpp"
#include "MathUtils.hpp"

namespace IWXMVM::MathUtils
{
	constexpr float PI = 3.14159265358979323846f;

	float DegreesToRadians(float degrees)
	{
		return degrees * (PI / 180.0f);
	}

	float RadiansToDegrees(float radians)
	{
		return radians * (180.0f / PI);
	}

	Vector3 ForwardVectorFromAngles(float pitch, float yaw, float roll)
	{
		auto p = MathUtils::DegreesToRadians(pitch);
		auto y = MathUtils::DegreesToRadians(yaw);
		return Vector3(
			std::cos(y) * std::cos(p),
			std::sin(y) * std::cos(p),
			-std::sin(p)
		);
	}

	Vector3 AnglesFromForwardVector(Vector3 forward)
	{
		auto yaw = std::atan2(forward.y, forward.x);
		auto pitch = std::atan2(-forward.z, std::sqrt(forward.x * forward.x + forward.y * forward.y));
		return Vector3(
			MathUtils::RadiansToDegrees(pitch),
			MathUtils::RadiansToDegrees(yaw),
			0.0f
		);
	}
}