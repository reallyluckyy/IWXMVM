#pragma once
#include "glm/vec3.hpp"

namespace IWXMVM::Types
{
    struct Filmtweaks
    {
        bool enabled;
        float brightness;
        float contrast;
        float desaturation;
        glm::vec3 tintLight;
        glm::vec3 tintDark;
        bool invert;
    };
}  // namespace IWXMVM::Types