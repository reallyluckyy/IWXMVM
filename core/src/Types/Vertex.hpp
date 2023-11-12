#pragma once

#include "StdInclude.hpp"

namespace IWXMVM::Types
{
    struct Vertex
    {
        glm::vec3 pos;
        D3DCOLOR col;
    };

    typedef std::uint32_t Index;
}  // namespace IWXMVM::Types