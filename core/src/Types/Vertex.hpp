#pragma once

#include "StdInclude.hpp"

namespace IWXMVM::Types
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        D3DCOLOR col;

        bool operator==(const Vertex& other) const noexcept
        {
            return pos == other.pos && normal == other.normal && col == other.col;
        }
    };

    struct FSVertex {
        glm::vec3 p;
        glm::vec2 uv;
    };

    typedef std::uint32_t Index;
}  // namespace IWXMVM::Types