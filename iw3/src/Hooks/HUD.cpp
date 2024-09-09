#include "StdInclude.hpp"
#include "HUD.hpp"

#include "Utilities/HookManager.hpp"
#include "../Addresses.hpp"
#include "../Structures.hpp"

namespace IWXMVM::IW3::Hooks::HUD
{
    bool showScore = true;
    bool showIconsAndText = true;

    typedef void(*R_AddCmdDrawTextWithEffects_t)(
        const char* text, int maxLength, int a3, float a4, float a5, float a6, float a7, float a8, float *color, int a10,
        float *a11, void *a12, void *a13, int a14, int a15, int a16, int a17);
    R_AddCmdDrawTextWithEffects_t R_AddCmdDrawTextWithEffects_Trampoline;
    void R_AddCmdDrawTextWithEffects_Hook(const char *text, int maxLength, int a3, float a4, float a5, float a6,
                                          float a7, float a8, float *color, int a10, float *a11, void *a12, 
                                          void *a13, int a14,int a15, int a16, int a17)
    {
        if (text[0] == '+')
        {
            if (!showScore)
                return;
        }
        else
        {
            if (!showIconsAndText)
                return;
        }

        R_AddCmdDrawTextWithEffects_Trampoline(text, maxLength, a3, a4, a5, a6, a7, a8, color, a10, a11, a12, a13, a14, a15, a16, a17);
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().R_AddCmdDrawTextWithEffects(),
                                (std::uintptr_t)R_AddCmdDrawTextWithEffects_Hook,
                                (uintptr_t*)&R_AddCmdDrawTextWithEffects_Trampoline);
    }
}  // namespace IWXMVM::IW3::Hooks::Playback
