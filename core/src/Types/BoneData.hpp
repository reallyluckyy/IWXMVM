#pragma once

#include <glm/glm.hpp>

namespace IWXMVM::Types
{
    struct BoneData
    {
        int32_t id;
        glm::vec3 position;
        glm::mat3x3 rotation;
    };
}  // namespace IWXMVM::Types