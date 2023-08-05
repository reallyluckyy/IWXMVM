#include "StdInclude.hpp"
#include "Camera.hpp"

namespace IWXMVM::Components
{
	bool Camera::IsModControlledCameraMode()
	{
		return mode != Mode::FirstPerson && mode != Mode::ThirdPerson;
	}
}