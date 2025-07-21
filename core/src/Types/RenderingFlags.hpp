#pragma once

namespace IWXMVM::Types
{
    enum RenderingFlags : uint32_t
    {
        RenderingFlags_Nothing = 0,

        RenderingFlags_DrawPlayers = 1 << 0,
        RenderingFlags_DrawWorld = 1 << 1,

        RenderingFlags_DrawEverything = 0xFFFFFFFF,
    };
}  // namespace IWXMVM::Types