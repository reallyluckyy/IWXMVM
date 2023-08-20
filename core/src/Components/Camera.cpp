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
		return MathUtils::ForwardVectorFromAngles(this->rotation[0], this->rotation[1], this->rotation[2]);
	}

	void Camera::SetForwardVector(Vector3 forward)
	{
		this->rotation = MathUtils::AnglesFromForwardVector(forward);
	}

	Vector3 Camera::GetRightVector()
	{
		return Vector3::Cross(GetForwardVector(), Vector3::Up) * -1;
	}
}