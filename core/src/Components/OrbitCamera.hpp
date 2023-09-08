#pragma once
#include "Camera.hpp"

namespace IWXMVM::Components
{
	class OrbitCamera
	{
	public:
		static OrbitCamera& Get()
		{
			static OrbitCamera instance;
			return instance;
		}

		OrbitCamera(OrbitCamera const&) = delete;
		void operator=(OrbitCamera const&) = delete;

		void UpdateMovement(Camera& camera);
	private:
		OrbitCamera() {}

		void DrawOrbitPoint(Camera& camera);
		void DrawGrid(Camera& camera);

		glm::vec3 orbitCameraOrigin {};
	};
}