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
		glm::vec3 position = point - camera.GetPosition();

		glm::vec3 transform;

		transform.x = glm::dot(position, camera.GetRightVector());
		transform.y = glm::dot(position, camera.GetUpVector());
		transform.z = glm::dot(position, camera.GetForwardVector());

		if (transform.z < 0.1f)
			return std::nullopt;

		uint32_t widthCenter = (uint32_t)((viewport.z) / 2.0f);
		uint32_t heightCenter = (uint32_t)((viewport.w) / 2.0f);

		float multiplier = 0.75f; //currentView.isPOV ? 1.0f : 0.75f;

		auto tanHalfFov = glm::tan(glm::radians(camera.GetFov()) * 0.5) * 0.75;
		auto aspectRatio = viewport.z / viewport.w;

		float x = (1 - (transform.x / (tanHalfFov * aspectRatio * multiplier) / transform.z));
		float y = (1 - (transform.y / (tanHalfFov * multiplier) / transform.z));

		return std::make_optional(ImVec2(widthCenter * x + viewport.x, heightCenter * y + viewport.y));
	}
}