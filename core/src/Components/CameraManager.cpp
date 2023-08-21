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

	void CameraManager::UpdateFreecamMovement()
	{
		//if (!Input::GetFocusedWindow() == GameView)
		//	return;

		auto& activeCamera = GetActiveCamera();

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

	void CameraManager::UpdateOrbitCameraMovement()
	{
		auto& activeCamera = GetActiveCamera();

		constexpr float BASE_SPEED = 0.1f;
		constexpr float ROTATION_SPEED = BASE_SPEED * 2.0f;
		constexpr float TRANSLATION_SPEED = BASE_SPEED * 3.0f;
		constexpr float ZOOM_SPEED = BASE_SPEED * 2.0f;

		// bump camera out of origin if it's at the origin
		if (activeCamera.GetPosition() == orbitCameraOrigin)
		{
			activeCamera.GetPosition() = orbitCameraOrigin + Vector3::One;
		}

		if (Input::KeyDown(ImGuiKey_F4))
		{
			orbitCameraOrigin = Vector3::Zero;
			activeCamera.GetPosition() = Vector3::One;
		}

		if (Input::MouseButtonHeld(ImGuiMouseButton_Middle))
		{
			auto horizontalDelta = -Input::GetMouseDelta()[0] * ROTATION_SPEED;
			activeCamera.GetPosition() -= orbitCameraOrigin;
			activeCamera.GetPosition().RotateAroundAxis(Vector3::Up, MathUtils::DegreesToRadians(horizontalDelta));
			activeCamera.GetPosition() += orbitCameraOrigin;

			auto verticalDelta = Input::GetMouseDelta()[1] * ROTATION_SPEED;
			activeCamera.GetPosition() -= orbitCameraOrigin;
			activeCamera.GetPosition().RotateAroundAxis(Vector3::Cross(Vector3::Up, activeCamera.GetForwardVector()), MathUtils::DegreesToRadians(verticalDelta));
			activeCamera.GetPosition() += orbitCameraOrigin;
		}

		if (Input::MouseButtonHeld(ImGuiMouseButton_Right))
		{
			Vector3 forward2D = activeCamera.GetForwardVector().Normalized();
			forward2D.z = 0;
			orbitCameraOrigin += forward2D * Input::GetMouseDelta()[1] * TRANSLATION_SPEED;
			activeCamera.GetPosition() += forward2D * Input::GetMouseDelta()[1] * TRANSLATION_SPEED;

			Vector3 right2D = activeCamera.GetRightVector().Normalized();
			right2D.z = 0;
			orbitCameraOrigin += right2D * Input::GetMouseDelta()[0] * TRANSLATION_SPEED;
			activeCamera.GetPosition() += right2D * Input::GetMouseDelta()[0] * TRANSLATION_SPEED;
		}


		activeCamera.SetForwardVector(orbitCameraOrigin - activeCamera.GetPosition());

		auto proximityDelta = -Input::GetScrollDelta() * ZOOM_SPEED;

		activeCamera.GetPosition() += (activeCamera.GetPosition() - orbitCameraOrigin).Normalized() * proximityDelta * 100;
	}


	void CameraManager::UpdateCameraFrame()
	{
		auto& activeCamera = GetActiveCamera();
		if (activeCamera.GetMode() == Camera::Mode::Free)
		{
			UpdateFreecamMovement();
		}
		else if (activeCamera.GetMode() == Camera::Mode::Orbit)
		{
			UpdateOrbitCameraMovement();
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
		for (size_t i = 0; i < cameras.size(); i++)
		{
			if (cameras[i].GetMode() == mode)
			{
				activeCameraIndex = i;
			}
		}

		Events::Invoke(EventType::OnCameraChanged);
	}
}