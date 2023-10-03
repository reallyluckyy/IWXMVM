#pragma once

#include <glm/vec3.hpp>

namespace IWXMVM::Types
{
    struct Marker
    {
        glm::vec3 position;
        glm::vec3 rotation;
        float fov;

        std::uint32_t tick;
    };
}  // namespace IWXMVM::Types