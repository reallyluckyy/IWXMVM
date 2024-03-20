#include "StdInclude.hpp"
#include "PlayerAnimation.hpp"

#include "Utilities/HookManager.hpp"
#include "../Addresses.hpp"
#include "Mod.hpp"
#include "Components/PlayerAnimation.hpp"

namespace IWXMVM::IW3::Hooks::PlayerAnimation
{
    void BG_RunLerpFrameRate_Hook_Internal(const std::uint32_t& curAnimIndex, std::uint32_t& newAnimIndex)
    {
        // bgs_t bg_s -> animScriptData_t animScriptData -> animation_t animations
        static constexpr std::size_t ANIM_NAME_OFFSET = 104; 
        static constexpr std::size_t ANIM_COUNT = 318;

        static const char* rootAnim = []() -> const char* 
        {
            const auto address = **reinterpret_cast<const char***>(GetGameAddresses().BG_RunLerpFrameRate() + 0x8B);
            const std::string_view sv{address, std::find(address, address + 5, '\0')};

            if (sv == "root")
            {
                std::vector<std::pair<std::string_view, std::uint32_t>> anims;
                anims.reserve(24);

                for (std::uint32_t i = 1; i < ANIM_COUNT; ++i)
                {
                    const auto ptr = sv.data() + i * ANIM_NAME_OFFSET;
                    const std::string_view animName(ptr, std::find(ptr, ptr + ANIM_NAME_OFFSET, '\0'));

                    if (animName.find("death") != std::string_view::npos)
                        anims.emplace_back(std::pair{animName, i});
                }

                if (anims.size() > 0)
                {
                    Components::PlayerAnimation::PopulateAnimationData(anims);
                    return sv.data();
                }
            }

            return nullptr;
        }();

        if (rootAnim != nullptr && newAnimIndex != curAnimIndex && (newAnimIndex % 512) < ANIM_COUNT)
        {
            const std::string_view animName(rootAnim + (newAnimIndex % 512) * ANIM_NAME_OFFSET);
            if (animName.find("death") != std::string_view::npos)
                Components::PlayerAnimation::SetPlayerAnimation(animName, newAnimIndex);
        }

        // promod animation bugs fix
        if (newAnimIndex != curAnimIndex && (newAnimIndex % 512 == 255 || newAnimIndex % 512 == 278))
            newAnimIndex = 0;
    }

    std::uintptr_t BG_RunLerpFrameRate_Trampoline;
    void __declspec(naked) BG_RunLerpFrameRate_Hook()
    {
        __asm
        {
            pushad
            lea ebx, [eax + 0x10]
            lea eax, [esp + 0x28]
            push eax
            push ebx
            call BG_RunLerpFrameRate_Hook_Internal
            add esp, 0x8
            popad
            jmp BG_RunLerpFrameRate_Trampoline
        }
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().BG_RunLerpFrameRate(), (std::uintptr_t)BG_RunLerpFrameRate_Hook,
                                &BG_RunLerpFrameRate_Trampoline);
    }
}  // namespace IWXMVM::IW3::Hooks::PlayerAnimation