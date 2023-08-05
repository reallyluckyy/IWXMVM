#pragma once
#include "Camera.hpp"

namespace IWXMVM::Components
{
	class CameraManager {

	public:
		void Initialize();

		Camera& GetActiveCamera() { return cameras[activeCameraIndex]; }
		Camera& GetCamera(Camera::Mode mode);
		void SetActiveCamera(Camera::Mode mode);

		std::string GetCameraModeLabel(Camera::Mode cameraMode);
		std::vector<Camera::Mode> GetCameraModes();

	private:
		void UpdateCameraFrame();

		std::vector<Camera> cameras = []() {
			// Create one camera for each camera mode
			std::vector<Camera> tmp;

			for (int i = 0; i < (int)Camera::Mode::Count; i++) {
				tmp.push_back(Camera((Camera::Mode)i));
			}

			return tmp;
		}();

		int activeCameraIndex = 0;
	};
}