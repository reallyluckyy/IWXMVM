#pragma once
#include "StdInclude.hpp"
#include "ShakeManager.hpp"

#include "Components/CameraManager.hpp"
#include "Mod.hpp"

namespace IWXMVM::Components
{
    glm::vec3 ShakeManager::CalculatePositionShake()
    {
        uint32_t tick = Mod::GetGameInterface()->GetDemoInfo().gameTick;

		glm::vec3 positionOffset = {};

		const auto& camera = CameraManager::Get().GetActiveCamera();
		const glm::vec3 forwardVec = camera->GetForwardVector();
		const glm::vec3 rightVec = camera->GetRightVector();
		const glm::vec3 upVec = glm::cross(forwardVec, rightVec);

		for (const auto& shake : shakes)
		{
			noise.SetSeed(shake.seed);
			float sample = (float)tick * shake.frequency;

			const float forwardsAmp = noise.GetNoise(sample, 100.0f) * shake.forwards - shake.forwards / 2.0f;
			const float sidewaysAmp = noise.GetNoise(sample, 600.0f) * shake.sideways - shake.sideways / 2.0f;
			const float verticalAmp = noise.GetNoise(sample, 1100.0f) * shake.vertical - shake.vertical / 2.0f;

			positionOffset += (forwardVec * forwardsAmp + rightVec * sidewaysAmp + upVec * verticalAmp) * shake.amplitude;
		}

		return positionOffset;
	}

	glm::vec3 ShakeManager::CalculateRotationShake()
	{
        uint32_t tick = Mod::GetGameInterface()->GetDemoInfo().gameTick;

		glm::vec3 rotationOffset = {};

		for (const auto& shake : shakes)
		{
			noise.SetSeed(shake.seed);
			float sample = (float)tick * shake.frequency;

			const float yawAmp = noise.GetNoise(sample, 1001.0f) * shake.yaw - shake.yaw / 2.0f;
			const float pitchAmp = noise.GetNoise(sample, 6001.0f) * shake.pitch - shake.pitch / 2.0f;
			const float rollAmp = noise.GetNoise(sample, 11001.0f) * shake.roll - shake.roll / 2.0f;

			rotationOffset += glm::vec3{yawAmp, pitchAmp, rollAmp} * shake.amplitude;
		}

		return rotationOffset;
	}
}