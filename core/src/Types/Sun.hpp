#pragma once
#include "glm/vec3.hpp"

namespace IWXMVM::Types
{
    struct Sun
    {
        glm::vec3 color;
        glm::vec3 direction;
        float brightness;
    };
}  // namespace IWXMVM::Types