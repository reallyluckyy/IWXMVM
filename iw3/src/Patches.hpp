#pragma once
#include "StdInclude.hpp"
#include "Addresses.hpp"
#include "../src/Utilities/Patches.hpp"

namespace IWXMVM::IW3::Patches
{
    using namespace IWXMVM::Patches;

    struct IW3Patches
    {
        static constexpr std::array CG_RegisterItemsBytes{
            "33 F6 2B D0 8A 08 88 0C 02 40 46 81 FE 80 00 00 00 7D 04 84 C9 75 ED BE 01 00 00 00 8D A4 24 00 00 00 "
            "00 8B C6 C1 E8 02 0F BE 44 04 08 83 F8 39 7F 05 83 E8 30 EB 03 83 E8 57 8B CE 83 E1 03 BA 01 00 00 00 "
            "D3 E2 85 D0 74 09 56 E8 1A F7 FF FF 83 C4 04 83 C6 01 81 FE 80 00 00 00 7C C5 5E 81 C4 88 00 00 00 C3 "_bytes};

        Patch<std::size(CG_RegisterItemsBytes)> CG_RegisterItems{GetGameAddresses().CG_RegisterItems(),
                                                                 CG_RegisterItemsBytes, PatchApplySetting::Immediately};
        ReturnPatch CG_DrawDisconnect{GetGameAddresses().CG_DrawDisconnect(), PatchApplySetting::Immediately};
        JumpPatch CG_AddPlayerSpriteDrawSurfs{GetGameAddresses().CG_AddPlayerSpriteDrawSurfs(),
                                              PatchApplySetting::Immediately};
        JumpPatch CL_CGameRendering{GetGameAddresses().CL_CGameRendering(), PatchApplySetting::Immediately};
        Patch<1> CL_KeyEvent{GetGameAddresses().CL_KeyEvent(), std::array<std::uint8_t, 1>{0x4E},
                             PatchApplySetting::Immediately};  // modify CL_KeyEvent to prevent the game hud from
                                                               // disappearing on left mouse click
        
        // For rewinding (not sure if all of these are actually necessary)
        NopPatch<5> Con_TimeJumped{GetGameAddresses().Con_TimeJumpedCall(), PatchApplySetting::Deferred};
        //NopPatch<2> CL_SetCGameTime{GetGameAddresses().CL_SetCGameTimeError(), PatchApplySetting::Immediately};
        //JumpPatch CL_CGameNeedsServerCommand{GetGameAddresses().CL_CGameNeedsServerCommandError(),
                                             //PatchApplySetting::Immediately};
        //JumpPatch CG_ProcessSnapshots{GetGameAddresses().CG_ProcessSnapshotsError(), PatchApplySetting::Immediately};
        //JumpPatch CG_ReadNextSnapshot{GetGameAddresses().CG_ReadNextSnapshotWarning(), PatchApplySetting::Immediately};
        NopPatch<5> CG_MapRestart{GetGameAddresses().CG_MapRestartSetThirdpersonCall(),
                                  PatchApplySetting::Immediately};  // Prevents cg_thirdperson from being reset

        // to remove the blood overlay when player gets hit
        NopPatch<5> CG_DrawPlayerLowHealthOverlay{GetGameAddresses().CG_DrawPlayerLowHealthOverlay(), PatchApplySetting::Deferred};
        ReturnPatch CG_DrawFlashDamage{GetGameAddresses().CG_DrawFlashDamage(), PatchApplySetting::Deferred};
        ReturnPatch CG_DrawDamageDirectionIndicators{GetGameAddresses().CG_DrawDamageDirectionIndicators(), PatchApplySetting::Deferred};
         
        ReturnPatch IN_Frame{GetGameAddresses().IN_Frame(), PatchApplySetting::Deferred};
    };

    inline IW3Patches& GetGamePatches()
    {
        static IW3Patches patches;
        return patches;
    }
}  // namespace IWXMVM::IW3::Patches
