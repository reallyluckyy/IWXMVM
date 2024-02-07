#include "StdInclude.hpp"
#include "Playback.hpp"

#include "Components/Playback.hpp"
#include "Utilities/HookManager.hpp"
#include "Events.hpp"
#include "../Addresses.hpp"

namespace IWXMVM::IW3::Hooks::Playback
{
    void SV_Frame_Internal(std::int32_t& msec)
    {
        msec = Components::Playback::CalculatePlaybackDelta(msec);
    }

    void __declspec(naked) SV_Frame_Hook()
    {
        static std::int32_t msec;
        static std::int32_t ebxValue;

        __asm
        {
			pop ecx
			pushad
			mov msec, esi
			mov ebxValue, ebx
        }

        if (ebxValue == *reinterpret_cast<std::int32_t*>(0x69136C))  // not true for CoD4X
        {
            SV_Frame_Internal(msec);
            __asm
            {
				popad
				mov eax, msec
				ret
            }
        }
        else
        {
            SV_Frame_Internal(msec);
            __asm
            {
				popad
				mov ebx, msec
				ret
            }
        }
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().SV_Frame(), (std::uintptr_t)SV_Frame_Hook, nullptr);
    }
}  // namespace IWXMVM::IW3::Hooks::Playback
