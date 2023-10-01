#pragma once
#include "Types/CampathNode.hpp"

namespace IWXMVM::Components
{
	enum class InterpolationMode
	{
		Linear,

		Count
	};

	class CampathManager 
	{

	public:
		static CampathManager& Get()
		{
			static CampathManager instance;
			return instance;
		}

		CampathManager(CampathManager const&) = delete;
		void operator=(CampathManager const&) = delete;

		void Initialize();
		void Update();

		const std::vector<Types::CampathNode>& GetNodes() const { return nodes; }
		void AddNode(Types::CampathNode marker);

		InterpolationMode GetInterpolationMode() const { return interpolationMode; }
		void SetInterpolationMode(InterpolationMode interpolationMode) { interpolationMode = interpolationMode; }
		std::string_view GetInterpolationModeLabel(InterpolationMode interpolationMode);
		std::vector<InterpolationMode> GetInterpolationModes();

	private:
		CampathManager() {}

		// Vector storing campath nodes in order of ticks
		std::vector<Types::CampathNode> nodes;

		InterpolationMode interpolationMode = InterpolationMode::Linear;
	};
}