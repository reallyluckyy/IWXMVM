#include "StdInclude.hpp"
#include "Camera.hpp"

#include "Utilities/MathUtils.hpp"

namespace IWXMVM::Components
{
	bool Camera::IsModControlledCameraMode()
	{
		return mode != Mode::FirstPerson && mode != Mode::ThirdPerson;
	}

	Vector3 Camera::GetForwardVector() 
	{
		return Vector3::FromAngles(this->rotation[0], this->rotation[1], this->rotation[2]);
	}

	Vector3 Camera::GetRightVector()
	{
		return Vector3::Cross(GetForwardVector(), Vector3::Up) * -1;
	}
}