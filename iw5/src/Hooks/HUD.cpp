#include "StdInclude.hpp"
#include "HUD.hpp"

#include "Utilities/HookManager.hpp"
#include "../Addresses.hpp"
#include "../Structures.hpp"

namespace IWXMVM::IW5::Hooks::HUD
{
    bool showShellshockEffects = true;

    bool showScore = true;
    bool showOtherText = true;

	typedef void (__cdecl * CG_DrawShellshockBlend_t)(int localClientNum);
    CG_DrawShellshockBlend_t CG_DrawShellshockBlend_Trampoline = nullptr;
    void CG_DrawShellshockBlend_Hook(int localClientNum)
	{
		if (showShellshockEffects)
		{
			CG_DrawShellshockBlend_Trampoline(localClientNum);
		}
	}

    typedef void (*R_AddCmdDrawText_t)(const char* text, int a2, int a3, float a4, float a5, float a6,
                                                      float a7, float a8, int a9, int a10);
    R_AddCmdDrawText_t R_AddCmdDrawText_Trampoline;
    void R_AddCmdDrawText_Hook(const char* text, int a2, int a3, float a4, float a5, float a6, float a7,
                                          float a8, int a9, int a10)
    {
        if (text[0] == '+')
        {
            if (!showScore)
                return;
        }
        else
        {
            if (!showOtherText)
                return;
        }

        R_AddCmdDrawText_Trampoline(text, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    typedef void (*R_AddCmdDrawTextWithEffects_t)(const char* text, int a2, int a3, float a4, float a5, float a6,
                                                  float a7, float a8, int a9, int a10, float *a11, char *a12, char *a13,
                                                  int a14, int a15, int a16, int a17);
    R_AddCmdDrawTextWithEffects_t R_AddCmdDrawTextWithEffects_Trampoline;
    void R_AddCmdDrawTextWithEffects_Hook(const char *text, int a2, int a3, float a4, float a5, float a6, float a7, float a8,
                               int a9, int a10, float *a11, char *a12, char *a13, int a14, int a15, int a16, int a17)
    {
        if (text[0] == '+')
        {
            if (!showScore)
                return;
        }
        else
        {
            if (!showOtherText)
                return;
        }

        R_AddCmdDrawTextWithEffects_Trampoline(text, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().CG_DrawShellshockBlend(), 
			(std::uintptr_t)CG_DrawShellshockBlend_Hook, (uintptr_t*)&CG_DrawShellshockBlend_Trampoline);

        HookManager::CreateHook(GetGameAddresses().R_AddCmdDrawText(),
                                (std::uintptr_t)R_AddCmdDrawText_Hook,
                                (uintptr_t*)&R_AddCmdDrawText_Trampoline);
        
        HookManager::CreateHook(GetGameAddresses().R_AddCmdDrawTextWithEffects(), (std::uintptr_t)R_AddCmdDrawTextWithEffects_Hook,
                                (uintptr_t*)&R_AddCmdDrawTextWithEffects_Trampoline);
    }
}  // namespace IWXMVM::IW3::Hooks::Playback
