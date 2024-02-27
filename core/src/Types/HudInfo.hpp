#pragma once
#include "glm/vec3.hpp"

namespace IWXMVM::Types
{
    struct HudInfo
    {
        bool show2DElements;
        bool showPlayerHUD;
        bool showShellshock;
        bool showCrosshair;
        bool showScore;

        bool showKillfeed;
        glm::vec3 killfeedTeam1Color;
        glm::vec3 killfeedTeam2Color;
    };
}  // namespace IWXMVM::Types