#include "StdInclude.hpp"
#include "DollyCamera.hpp"

#include "Mod.hpp"

namespace IWXMVM::Components
{
	void DollyCamera::Initialize()
	{

	}

	Types::CampathNode LinearlyInterpolate(const std::vector<Types::CampathNode>& campathNodes, uint32_t tick) 
	{
		// We want to know the campathNodes that the current tick is sandwiched between
		// p0 will be the node before the current tick, p1 the node after the current tick

		std::size_t p0Idx = 0, p1Idx = 1;
		for (std::size_t i = 0; i < campathNodes.size() - 1; i++)
		{
			if (tick >= campathNodes[i].tick && tick <= campathNodes[i + 1].tick)
			{
				p0Idx = i;
				p1Idx = i + 1;
				break;
			}
		}

		const auto& p0 = campathNodes[p0Idx];
		const auto& p1 = campathNodes[p1Idx];

		// Interpolate between p0 and p1
		const float t = static_cast<float>(tick - p0.tick) / static_cast<float>(p1.tick - p0.tick);
		return Types::CampathNode(
			(1.0f - t) * p0.position + t * p1.position,
			(1.0f - t) * p0.rotation + t * p1.rotation,
			(1.0f - t) * p0.fov + t * p1.fov,
			tick
		);
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

		Types::CampathNode interpolatedNode;
		switch (CampathManager::Get().GetInterpolationMode())
		{
			case InterpolationMode::Linear:
				interpolatedNode = LinearlyInterpolate(nodes, currentTick);
				break;
			default:
				throw std::runtime_error("Unknown interpolation mode");
		}

		this->GetPosition() = interpolatedNode.position;
		this->GetRotation() = interpolatedNode.rotation;
		this->GetFov() = interpolatedNode.fov;
	}
}
