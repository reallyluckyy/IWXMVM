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
}