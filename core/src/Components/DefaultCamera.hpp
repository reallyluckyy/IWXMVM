#pragma once
#include "Camera.hpp"

namespace IWXMVM::Components
{
	class DefaultCamera : public Camera
	{
	public:
		DefaultCamera(Camera::Mode mode) 
		{
			this->mode = mode;
		}

		void Initialize() override {};
		void Update() override {};
	};
}