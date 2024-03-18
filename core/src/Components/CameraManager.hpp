#pragma once
#include "Camera.hpp"
#include "DefaultCamera.hpp"
#include "DollyCamera.hpp"
#include "FreeCamera.hpp"
#include "OrbitCamera.hpp"
#include "BoneCamera.hpp"

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

        std::unique_ptr<Camera>& GetActiveCamera()
        {
            return cameras[activeCameraIndex];
        }
        std::unique_ptr<Camera>& GetPreviousActiveCamera()
        {
            return cameras[previousActiveCameraIndex];
        }
        std::unique_ptr<Camera>& GetCamera(Camera::Mode mode);
        void SetActiveCamera(Camera::Mode mode);

        std::string_view GetCameraModeLabel(Camera::Mode cameraMode);
        std::vector<Camera::Mode> GetCameraModes();

       private:
        CameraManager()
        {
        }

        void UpdateCameraFrame();

        std::vector<std::unique_ptr<Camera>> cameras = []() {
            // Create one camera for each camera mode
            std::vector<std::unique_ptr<Camera>> tmp;

            tmp.push_back(std::make_unique<DefaultCamera>(DefaultCamera(Camera::Mode::FirstPerson)));
            tmp.push_back(std::make_unique<DefaultCamera>(DefaultCamera(Camera::Mode::ThirdPerson)));
            tmp.push_back(std::make_unique<FreeCamera>(FreeCamera()));
            tmp.push_back(std::make_unique<OrbitCamera>(OrbitCamera()));
            tmp.push_back(std::make_unique<DollyCamera>(DollyCamera()));
            tmp.push_back(std::make_unique<BoneCamera>(BoneCamera()));

            if (tmp.size() != (int)Camera::Mode::Count)
            {
                LOG_ERROR("CameraManager::cameras is not the same size as Camera::Mode");
            }

            return tmp;
        }();

        int activeCameraIndex = 0;
        int previousActiveCameraIndex = static_cast<int>(Camera::Mode::Free); // setting free for FirstPersonToggle
    };
}  // namespace IWXMVM::Components