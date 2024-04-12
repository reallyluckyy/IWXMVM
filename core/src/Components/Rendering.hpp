#pragma once
#include "Types/RenderingFlags.hpp"

namespace IWXMVM::Components::Rendering
{
    Types::RenderingFlags GetRenderingFlags();
    void SetRenderingFlags(Types::RenderingFlags);

    void Initialize();
}  // namespace IWXMVM::Components::Rendering