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
};

inline IW3Patches& GetGamePatches()
{
    static IW3Patches patches;
    return patches;
}
}  // namespace IWXMVM::IW3::Patches
