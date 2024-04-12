#pragma once

namespace IWXMVM::Types
{
    enum RenderingFlags : uint32_t
    {
        RenderingFlags_Nothing = 0,

        RenderingFlags_DrawPlayers = 1 << 0,
        RenderingFlags_DrawWorld = 1 << 1,
        RenderingFlags_DrawViewmodel = 1 << 2,
        RenderingFlags_DrawMuzzleFlash = 1 << 3,

        RenderingFlags_DrawEverything = 0xFFFFFFFF,
    };
}  // namespace IWXMVM::Types