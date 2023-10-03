#include "StdInclude.hpp"
#include "DollyCamera.hpp"

#include "Mod.hpp"

namespace IWXMVM::Components
{
	void DollyCamera::Initialize()
	{

	}

	void DollyCamera::Update()
	{
		const auto& nodes = CampathManager::Get().GetNodes();

		if (nodes.size() == 0)
		{
			return;
		}

		const auto& firstNode = nodes.front();
		const auto& lastNode = nodes.back();

		const auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;

		if (currentTick <= firstNode.tick)
		{
			this->GetPosition() = firstNode.position;
			this->GetRotation() = firstNode.rotation;
			this->GetFov() = firstNode.fov;

			return;
		}
		else if (currentTick >= lastNode.tick)
		{
			this->GetPosition() = lastNode.position;
			this->GetRotation() = lastNode.rotation;
			this->GetFov() = lastNode.fov;

			return;
		}

		auto interpolatedNode = CampathManager::Get().Interpolate(currentTick);

		this->GetPosition() = interpolatedNode.position;
		this->GetRotation() = interpolatedNode.rotation;
		this->GetFov() = interpolatedNode.fov;
	}
}
