#pragma once

#include <glm/vec3.hpp>

namespace IWXMVM::Types
{
	struct CampathNode
	{
		glm::vec3 position;
		glm::vec3 rotation;
		float fov;

		std::uint32_t tick;

		CampathNode() = default;
		CampathNode(glm::vec3 position, glm::vec3 rotation, float fov, std::uint32_t tick) : position(position), rotation(rotation), fov(fov), tick(tick) {}
	};
}