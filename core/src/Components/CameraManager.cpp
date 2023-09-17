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
		if (Mod::GetGameInterface()->GetGameState() == Types::GameState::MainMenu)
		{
			return;
		}

		GetActiveCamera()->Update();

		// Modifying markers shouldn't be possible while in Dolly mode
		if (GetActiveCamera()->GetMode() != Camera::Mode::Dolly)
		{
			if (Input::BindDown("dollyAddMarker"))
			{
				Types::Marker marker{};
				marker.position = GetActiveCamera()->GetPosition();
				marker.rotation = GetActiveCamera()->GetRotation();
				marker.fov = GetActiveCamera()->GetFov();
				marker.tick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
				AddMarker(marker);

				LOG_DEBUG("Placed marker at (x: {}; y: {}; z: {}) with (pitch: {}; yaw: {}; roll: {}) at tick {}", marker.position.x, marker.position.y, marker.position.z, marker.rotation.x, marker.rotation.y, marker.rotation.z, marker.tick);
			}

			if (Input::BindDown("dollyClearMarkers"))
			{
				markers.clear();

				LOG_DEBUG("Markers cleared");
			}

			if (Input::BindDown("dollyPlayPath"))
			{
				SetActiveCamera(Camera::Mode::Dolly);
			}
		}
	}

	void CameraManager::Initialize()
	{
		for (auto& camera : cameras)
		{
			camera->Initialize();
		}

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

	void CameraManager::AddMarker(Types::Marker marker)
	{
		// If there's another marker at this tick already, overwrite it
		for (auto& m : markers)
		{
			if (m.tick == marker.tick)
			{
				m = marker;
				return;
			}
		}

		markers.push_back(marker);

		// Vector needs to always be sorted in ascending order by marker ticks
		std::sort(markers.begin(), markers.end(), [](const Types::Marker& a, const Types::Marker& b)
			{
				return a.tick < b.tick;
			});
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
}
