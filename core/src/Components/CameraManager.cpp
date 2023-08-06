#include "StdInclude.hpp"
#include "CameraManager.hpp"

#include "../Events.hpp"
#include "Mod.hpp"

namespace IWXMVM::Components
{
	std::string CameraManager::GetCameraModeLabel(Camera::Mode cameraMode)
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
		auto& activeCamera = GetActiveCamera();
		// TODO: move to input system
		if (GetActiveCamera().GetMode() == Camera::Mode::Free)
		{
			if (GetAsyncKeyState(0x57)) // W
				activeCamera.GetPosition()[0] += 1;

			if (GetAsyncKeyState(0x53)) // S
				activeCamera.GetPosition()[0] -= 1;

			if (GetAsyncKeyState(0x41)) // A
				activeCamera.GetRotation()[1] -= 1;

			if (GetAsyncKeyState(0x44)) // D
				activeCamera.GetRotation()[1] += 1;

			if (GetAsyncKeyState(VK_SPACE)) // SPACE
				activeCamera.GetPosition()[2] += 1;
		}
	}

	void CameraManager::Initialize()
	{
		Events::RegisterListener(EventType::OnFrame, [&]()
			{
				UpdateCameraFrame();
			}
		);

		Events::RegisterListener(EventType::OnDemoLoad, []()
			{
				// TODO: set initial coordinates of "free"-type cameras based on map
			}
		);
	}

	Camera& CameraManager::GetCamera(Camera::Mode mode)
	{
		for (auto& camera : cameras)
		{
			if (camera.GetMode() == mode)
			{
				return camera;
			}
		}

		throw std::runtime_error("Failed to find camera with desired mode");
	}

	void CameraManager::SetActiveCamera(Camera::Mode mode)
	{
		for (int i = 0; i < cameras.size(); i++)
		{
			if (cameras[i].GetMode() == mode)
			{
				activeCameraIndex = i;
			}
		}

		Events::Invoke(EventType::OnCameraChanged);
	}
}