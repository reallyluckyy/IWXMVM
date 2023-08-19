#include "StdInclude.hpp"
#include "CameraManager.hpp"

#include "../Events.hpp"
#include "../Input.hpp"
#include "Mod.hpp"

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

	void UpdateFreecamMovement(Camera& activeCamera)
	{
		//if (!Input::GetFocusedWindow() == GameView)
		//	return;

		// TODO: make this configurable
		constexpr float FREECAM_SPEED = 200;
		constexpr float MOUSE_SPEED = 0.1f;

		auto cameraSpeed = FREECAM_SPEED * Input::GetDeltaTime();

		if (Input::KeyHeld(ImGuiKey_W))
		{
			activeCamera.GetPosition() += activeCamera.GetForwardVector() * cameraSpeed;
		}

		if (Input::KeyHeld(ImGuiKey_S))
		{
			activeCamera.GetPosition() -= activeCamera.GetForwardVector() * cameraSpeed;
		}

		if (Input::KeyHeld(ImGuiKey_A))
		{
			activeCamera.GetPosition() += activeCamera.GetRightVector() * cameraSpeed;
		}

		if (Input::KeyHeld(ImGuiKey_D))
		{
			activeCamera.GetPosition() -= activeCamera.GetRightVector() * cameraSpeed;
		}

		activeCamera.GetRotation()[0] += Input::GetMouseDelta()[1] * MOUSE_SPEED;
		activeCamera.GetRotation()[1] -= Input::GetMouseDelta()[0] * MOUSE_SPEED;

		if (Input::KeyHeld(ImGuiKey_Space))
			activeCamera.GetPosition()[2] += cameraSpeed;
	}

	void CameraManager::UpdateCameraFrame()
	{
		auto& activeCamera = GetActiveCamera();
		if (activeCamera.GetMode() == Camera::Mode::Free)
		{
			UpdateFreecamMovement(activeCamera);
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