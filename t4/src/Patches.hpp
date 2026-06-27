#pragma once
#include "StdInclude.hpp"
#include "Addresses.hpp"
#include "../src/Utilities/Patches.hpp"

namespace IWXMVM::T4::Patches
{
    using namespace IWXMVM::Patches;

    struct T4Patches
    {
        ReturnPatch CG_DrawDisconnect{GetGameAddresses().CG_DrawDisconnect(), PatchApplySetting::Immediately};
        
        JumpPatch CG_AddPlayerSpriteDrawSurfs{GetGameAddresses().CG_AddPlayerSpriteDrawSurfs(),
                                              PatchApplySetting::Immediately};
        
        JumpPatch CL_CGameRendering{GetGameAddresses().CL_CGameRendering(), PatchApplySetting::Immediately};

        // Ensure the depth buffer is always filled
        ReturnValuePatch<1> R_DoesDrawSurfListInfoNeedFloatz{GetGameAddresses().R_DoesDrawSurfListInfoNeedFloatz(),
                                                             PatchApplySetting::Immediately};

        // Modify CL_KeyEvent to prevent the game hud from disappearing on left mouse click
        Patch<1> CL_KeyEvent{GetGameAddresses().CL_KeyEvent(), std::array<std::uint8_t, 1>{0xB8 + 12},
                             PatchApplySetting::Immediately};
        
        // For rewinding (not sure if all of these are actually necessary)
        NopPatch<5> Con_TimeJumped{GetGameAddresses().Con_TimeJumpedCall(), PatchApplySetting::Deferred};

        // Prevents cg_thirdperson from being reset
        NopPatch<5> CG_MapRestart{GetGameAddresses().CG_MapRestartSetThirdpersonCall(),
                                  PatchApplySetting::Immediately};

        // to remove the blood overlay when player gets hit
        NopPatch<5> CG_DrawPlayerLowHealthOverlay{GetGameAddresses().CG_DrawPlayerLowHealthOverlay(), PatchApplySetting::Deferred};
        ReturnPatch CG_DrawFlashDamage{GetGameAddresses().CG_DrawFlashDamage(), PatchApplySetting::Deferred};
        ReturnPatch CG_DrawDamageDirectionIndicators{GetGameAddresses().CG_DrawDamageDirectionIndicators(), PatchApplySetting::Deferred};
         
        ReturnPatch IN_Frame{GetGameAddresses().IN_Frame(), PatchApplySetting::Deferred};
    };

    inline T4Patches& GetGamePatches()
    {
        static T4Patches patches;
        return patches;
    }
}  // namespace IWXMVM::T4::Patches
