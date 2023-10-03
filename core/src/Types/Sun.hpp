#pragma once
#include "glm/vec3.hpp"

namespace IWXMVM::Types
{
struct Sun
{
    glm::vec3 color;
    glm::vec3 position;
    float brightness;
};
}  // namespace IWXMVM::Types