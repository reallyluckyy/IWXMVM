#pragma once
#include "StdInclude.hpp"

namespace IWXMVM::Components
{
	struct ShakeSettings
	{
		float amplitude = 0.2f;
		float frequency = 0.1f;
		float forwards;
		float sideways;
		float vertical;
		float yaw;
		float pitch;
		float roll;
        int32_t seed;
	};

	class ShakeManager
	{
		public:
			static ShakeManager& Get()
			{
				static ShakeManager instance;
				return instance;
		    }

			ShakeManager(ShakeManager const&) = delete;
			void operator=(ShakeManager const&) = delete;

			glm::vec3 CalculatePositionShake();
			glm::vec3 CalculateRotationShake();

			std::array<ShakeSettings, 4> shakes;
		private:
			ShakeManager()
			{
				noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
			}

			FastNoiseLite noise;
	};
}