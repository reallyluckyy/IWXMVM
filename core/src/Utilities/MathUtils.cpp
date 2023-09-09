#include "StdInclude.hpp"
#include "MathUtils.hpp"

namespace IWXMVM::MathUtils
{
	glm::vec3 ForwardVectorFromAngles(float pitch, float yaw, float roll)
	{
		auto p = glm::radians(pitch);
		auto y = glm::radians(yaw);
		return glm::vec3(
			std::cos(y) * std::cos(p),
			std::sin(y) * std::cos(p),
			-std::sin(p)
		);
	}

	glm::vec3 AnglesFromForwardVector(glm::vec3 forward)
	{
		auto yaw = std::atan2(forward.y, forward.x);
		auto pitch = std::atan2(-forward.z, std::sqrt(forward.x * forward.x + forward.y * forward.y));
		return glm::vec3(
			glm::degrees(pitch),
			glm::degrees(yaw),
			0.0f
		);
	}

	std::optional<ImVec2> WorldToScreenPoint(glm::vec3 point, Components::Camera& camera, glm::vec4 viewport)
	{
		auto lookat = glm::lookAtLH(camera.GetPosition(), camera.GetPosition() + camera.GetForwardVector(), glm::vector3::up);

		// this is quite the magic number, but it seems to be the scaling factor necessary to line this up with the previous world to screen implementation
		const auto MAGIC_NUMBER = 0.65f;
		auto proj  = glm::project(point, (lookat), glm::perspectiveFov(glm::radians(camera.GetFov() * 0.65f), viewport.z, viewport.w, 0.1f, 1000.0f), viewport);

		return std::make_optional(ImVec2(proj.x, proj.y));
	}
}