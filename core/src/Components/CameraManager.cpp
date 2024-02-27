#include "StdInclude.hpp"
#include "CameraManager.hpp"

#include "../Events.hpp"
#include "../Input.hpp"
#include "Mod.hpp"
#include "Utilities/MathUtils.hpp"

namespace IWXMVM::Components
{
    std::string_view CameraManager::GetCameraModeLabel(Camera::Mode cameraMode)
    {
        switch (cameraMode)
        {
            case Camera::Mode::FirstPerson:
                return "First Person Camera";
            case Camera::Mode::ThirdPerson:
                return "Third Person Camera";
            case Camera::Mode::Free:
                return "Free Camera";
            case Camera::Mode::Orbit:
                return "Orbit Camera";
            case Camera::Mode::Dolly:
                return "Dolly Camera";
            case Camera::Mode::Bone:
                return "Bone Camera";
            default:
                return "Unknown Camera Mode";
        }
    }

    std::vector<Camera::Mode> CameraManager::GetCameraModes()
    {
        std::vector<Camera::Mode> cameraModes;
        for (int i = 0; i < (int)Camera::Mode::Count; i++)
        {
            cameraModes.push_back((Camera::Mode)i);
        }
        return cameraModes;
    }

    void CameraManager::UpdateCameraFrame()
    {
        if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
        {
            return;
        }

        GetActiveCamera()->Update();
    }

    void CameraManager::Initialize()
    {
        for (auto& camera : cameras)
        {
            camera->Initialize();
        }

        Events::RegisterListener(EventType::OnFrame, [&]() { UpdateCameraFrame(); });

        Events::RegisterListener(EventType::PostDemoLoad, [&]() { SetActiveCamera(Camera::Mode::FirstPerson); });

        Events::RegisterListener(EventType::OnCameraChanged, [&]() {
            auto& activeCamera = GetActiveCamera();

            auto& firstPersonCamera = GetCamera(Camera::Mode::FirstPerson);
            switch (activeCamera->GetMode())
            {
                case Camera::Mode::Free:
                {
                    activeCamera->GetPosition() =
                        firstPersonCamera->GetPosition() - firstPersonCamera->GetForwardVector() * 100;
                    activeCamera->GetRotation() = firstPersonCamera->GetRotation();
                    break;
                }
                default:
                    break;
            }

        });
    }

    std::unique_ptr<Camera>& CameraManager::GetCamera(Camera::Mode mode)
    {
        for (auto& camera : cameras)
        {
            if (camera->GetMode() == mode)
            {
                return camera;
            }
        }

        throw std::runtime_error("Failed to find camera with desired mode");
    }

    void CameraManager::SetActiveCamera(Camera::Mode mode)
    {
        for (size_t i = 0; i < cameras.size(); i++)
        {
            if (cameras[i]->GetMode() == mode)
            {
                activeCameraIndex = i;
            }
        }

        Events::Invoke(EventType::OnCameraChanged);
    }
}  // namespace IWXMVM::Components
