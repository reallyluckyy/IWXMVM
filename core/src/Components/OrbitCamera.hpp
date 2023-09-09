#pragma once
#include "Camera.hpp"

namespace IWXMVM::Components
{
	class OrbitCamera : public Camera
	{
	public:
		OrbitCamera()
		{
			this->mode = Camera::Mode::Orbit;
		}

		void Update() override;
	private:

		void DrawOrbitPoint();
		void DrawGrid();

		glm::vec3 orbitCameraOrigin {};
	};
}