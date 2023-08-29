#pragma once
#include "Camera.hpp"

namespace IWXMVM::Components
{
	class CameraManager 
    {

	public:
        static CameraManager& Get()
        {
            static CameraManager instance;
            return instance;
        }

        CameraManager(CameraManager const&) = delete;
        void operator=(CameraManager const&) = delete;

		void Initialize();

		Camera& GetActiveCamera() { return cameras[activeCameraIndex]; }
		Camera& GetCamera(Camera::Mode mode);
		void SetActiveCamera(Camera::Mode mode);

		std::string_view GetCameraModeLabel(Camera::Mode cameraMode);
		std::vector<Camera::Mode> GetCameraModes();

	private:
        CameraManager() {}

		void UpdateFreecamMovement();
		void UpdateOrbitCameraMovement();

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

		glm::vec3 orbitCameraOrigin {};
	};
}