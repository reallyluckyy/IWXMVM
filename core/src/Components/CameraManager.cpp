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

	void CameraManager::UpdateFreecamMovement()
	{
		//if (!Input::GetFocusedWindow() == GameView)
		//	return;
	
		auto& activeCamera = GetActiveCamera();
		auto& cameraPosition = activeCamera.GetPosition();
	
		// TODO: make this configurable
		constexpr float FREECAM_SPEED = 300;
		constexpr float MOUSE_SPEED = 0.1f;
		constexpr float SCROLL_LOWER_BOUNDARY = -0.001f;
		constexpr float SCROLL_UPPER_BOUNDARY = 0.001f;
		const float SMOOTHING_FACTOR = glm::clamp(1.0f - 15.0f * Input::GetDeltaTime(), 0.0f, 1.0f);

		static double scrollDelta = 0.0;

		auto speedModifier = Input::KeyHeld(ImGuiKey_LeftShift) ? 0.1f : 1.0f;
		speedModifier *= Input::KeyHeld(ImGuiKey_LeftCtrl) ? 3.0f : 1.0f;

		const auto cameraBaseSpeed = Input::GetDeltaTime() * FREECAM_SPEED;
		const auto cameraMovementSpeed = cameraBaseSpeed * speedModifier;
		const auto cameraHeightSpeed = cameraMovementSpeed;

		if (Input::KeyHeld(ImGuiKey_W))
		{
			cameraPosition += activeCamera.GetForwardVector() * cameraMovementSpeed;
		}
	
		if (Input::KeyHeld(ImGuiKey_S))
		{
			cameraPosition -= activeCamera.GetForwardVector() * cameraMovementSpeed;
		}
	
		if (Input::KeyHeld(ImGuiKey_A))
		{
			cameraPosition += activeCamera.GetRightVector() * cameraMovementSpeed;
		}
	
		if (Input::KeyHeld(ImGuiKey_D))
		{
			cameraPosition -= activeCamera.GetRightVector() * cameraMovementSpeed;
		}

		if (Input::KeyHeld(ImGuiKey_LeftAlt))
		{
			activeCamera.GetRotation()[2] += Input::GetScrollDelta();
		}
		else
		{
			scrollDelta -= Input::GetScrollDelta();

			if (scrollDelta < SCROLL_LOWER_BOUNDARY || scrollDelta > SCROLL_UPPER_BOUNDARY)
			{
				activeCamera.GetFov() += scrollDelta * Input::GetDeltaTime() * 32.0f;
				activeCamera.GetFov() = glm::clamp(activeCamera.GetFov(), 1.0f, 179.0f);
				scrollDelta *= SMOOTHING_FACTOR;
			}
			else if (scrollDelta != 0.0)
			{
				scrollDelta = 0.0;
			}
		}

		if (Input::MouseButtonHeld(ImGuiMouseButton_Middle)) 
		{
			activeCamera.GetRotation() = {};
			activeCamera.GetFov() = 90.0f;
		}
	
		activeCamera.GetRotation()[0] += Input::GetMouseDelta()[1] * MOUSE_SPEED;
		activeCamera.GetRotation()[1] -= Input::GetMouseDelta()[0] * MOUSE_SPEED;

		activeCamera.GetRotation()[0] = glm::clamp(activeCamera.GetRotation()[0], -89.9f, 89.9f);
	
		if (Input::KeyHeld(ImGuiKey_E))
			cameraPosition[2] += cameraHeightSpeed;
	
		if (Input::KeyHeld(ImGuiKey_Q))
			cameraPosition[2] -= cameraHeightSpeed;
	}

	void CameraManager::UpdateCameraFrame()
	{
		if (Mod::GetGameInterface()->GetGameState() == Types::GameState::MainMenu) 
		{
			return;
		}
		
		auto& activeCamera = GetActiveCamera();
		if (activeCamera.GetMode() == Camera::Mode::Free)
		{
			UpdateFreecamMovement();
		}
		else if (activeCamera.GetMode() == Camera::Mode::Orbit)
		{
			OrbitCamera::Get().UpdateMovement(activeCamera);
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
