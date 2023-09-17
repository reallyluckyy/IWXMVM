#pragma once
#include "StdInclude.hpp"
#include "Addresses.hpp"
#include "../src/Utilities/Patches.hpp"

namespace IWXMVM::IW3::Patches
{
    using namespace IWXMVM::Patches;

    struct IW3Patches
    {
        ReturnPatch CG_DrawDisconnect{ GetGameAddresses().CG_DrawDisconnect(), PatchApplySetting::Immediately };
    };

    inline IW3Patches& GetGamePatches()
    {
        static IW3Patches patches;
        return patches;
    }
}