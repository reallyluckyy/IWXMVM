#include "StdInclude.hpp"
#include "CampathManager.hpp"

#include "Mod.hpp"
#include "../Events.hpp"
#include "../Input.hpp"

namespace IWXMVM::Components
{
	std::string_view CampathManager::GetInterpolationModeLabel(InterpolationMode interpolationMode)
	{
		switch (interpolationMode)
		{
			case InterpolationMode::Linear:
				return "Linear";
			default:
				return "Unknown";
		}
	}

	std::vector<InterpolationMode> CampathManager::GetInterpolationModes()
	{
		std::vector<InterpolationMode> interpolationModes;
		for (int i = 0; i < (int)InterpolationMode::Count; i++)
		{
			interpolationModes.push_back((InterpolationMode)i);
		}
		return interpolationModes;
	}

	void CampathManager::Update()
	{
		if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
		{
			return;
		}

		auto& activeCamera = CameraManager::Get().GetActiveCamera();

		// Modifying nodes shouldn't be possible while in Dolly mode
		// TODO: There should be better feedback for making it clear to the user that they can place nodes (from their current camera position)
		// TOOD: while in modes that are not the usual freecam. Its a bit unintuitive being able to place nodes while in POV for example.
		if (activeCamera->GetMode() != Camera::Mode::Dolly)
		{
			if (Input::BindDown(InputConfiguration::BIND_DOLLY_ADD_NODE))
			{
				Types::CampathNode node{};
				node.position = activeCamera->GetPosition();
				node.rotation = activeCamera->GetRotation();
				node.fov = activeCamera->GetFov();
				node.tick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
				AddNode(node);

				LOG_DEBUG("Placed node at (x: {}; y: {}; z: {}) with (pitch: {}; yaw: {}; roll: {}) at tick {}", node.position.x, node.position.y, node.position.z, node.rotation.x, node.rotation.y, node.rotation.z, node.tick);
			}

			if (Input::BindDown(InputConfiguration::BIND_DOLLY_CLEAR_NODES))
			{
				nodes.clear();

				LOG_DEBUG("Nodes cleared");
			}

			if (Input::BindDown(InputConfiguration::BIND_DOLLY_PLAY_PATH))
			{
				CameraManager::Get().SetActiveCamera(Camera::Mode::Dolly);
			}
		}
	}


	void CampathManager::AddNode(Types::CampathNode node)
	{
		// If there's another node at this tick already, overwrite it
		// TODO: Do we want this behaviour? I feel like there should be a warning prompt here
		for (auto& m : nodes)
		{
			if (m.tick == node.tick)
			{
				m = node;
				return;
			}
		}

		nodes.push_back(node);

		// Vector needs to always be sorted in ascending order by node ticks
		std::sort(nodes.begin(), nodes.end(), [](const Types::CampathNode& a, const Types::CampathNode& b)
			{
				return a.tick < b.tick;
			});
	}

	void CampathManager::Initialize()
	{
		Events::RegisterListener(EventType::OnFrame, [&]()
			{
				Update();
			}
		);
	}
}
