#include "StdInclude.hpp"
#include "PlayerAnimation.hpp"

#include "../Addresses.hpp"
#include "../Structures.hpp"
#include "Components/PlayerAnimation.hpp"
#include "Mod.hpp"
#include "Utilities/HookManager.hpp"

namespace IWXMVM::IW3::Hooks::PlayerAnimation
{    
    inline constexpr std::size_t ANIM_COUNT = 318;

    bool GetAnimationNames()
    {
        const auto animations = std::span{Structures::GetClientGlobals()->bgs.animScriptData.animations};
        std::vector<std::pair<std::string_view, std::uint32_t>> anims;
        anims.reserve(24);

        for (std::uint32_t i = 1; i < std::min(ANIM_COUNT, animations.size()); ++i)
        {
            const std::string_view animName{animations[i].name};
            if (animName.find("death") != std::string_view::npos)
            {
                anims.emplace_back(std::pair{animName, i});
            }
        }

        if (anims.size() > 0)
        {
            Components::PlayerAnimation::PopulateAnimationData(anims);
            return true;
        }

        LOG_ERROR("Getting death animation names was unsuccessful!");
        return false;
    }

    void CG_ProcessEntity_Hook_Internal(Structures::centity_s& centity)
    {
        using namespace Structures;

        static bool foundDeathAnimations = GetAnimationNames();

        if (centity.nextState.eType == entityType_t::ET_PLAYER)
        {
            auto& torsoAnim = reinterpret_cast<std::uint32_t&>(centity.nextState.torsoAnim);

            if ((torsoAnim & 511) == 255 || (torsoAnim & 511) == 278)
            {
                // promod animation bugs fix
                torsoAnim = 0;
            }
        }
        else if (centity.nextState.eType == entityType_t::ET_PLAYER_CORPSE)
        {
            auto& legsAnim = reinterpret_cast<std::uint32_t&>(centity.nextState.legsAnim);

            if ((legsAnim & 511) != 0 && foundDeathAnimations)
            {
                const auto animations = std::span{GetClientGlobals()->bgs.animScriptData.animations};

                if ((legsAnim & 511) < std::min(ANIM_COUNT, animations.size()))
                {
                    const std::string_view animName{animations[(legsAnim & 511)].name};
                    if (animName.find("death") != std::string_view::npos)
                    {
                        // replace death animation
                        Components::PlayerAnimation::SetPlayerAnimation(animName, legsAnim);
                    } 
                }
            }

            if (Components::PlayerAnimation::AttachWeaponToCorpse()) 
            {
                if (const auto* centities = GetEntities(); static_cast<std::size_t>(centity.nextState.clientNum) < 64)
                {
                    if (centity.nextState.clientNum == centities[centity.nextState.clientNum].nextState.clientNum)
                    {
                        // attach weapon to corpse
                        centity.nextState.weapon = centities[centity.nextState.clientNum].nextState.weapon;
                    }
                }
            }
        }
        else if (centity.nextState.eType == entityType_t::ET_ITEM)
        {
            if (Components::PlayerAnimation::AttachWeaponToCorpse())
            {
                // hide dropped weapons
                centity.nextState.lerp.eFlags = 32;
            }
        }
    }

    std::uintptr_t CG_ProcessEntity_Trampoline;
    void __declspec(naked) CG_ProcessEntity_Hook()
    {
        __asm
        {
            pushad
            push esi
            call CG_ProcessEntity_Hook_Internal
            add esp, 0x4
            popad
            jmp CG_ProcessEntity_Trampoline
        }
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().CG_ProcessEntity(), (std::uintptr_t)CG_ProcessEntity_Hook,
                                &CG_ProcessEntity_Trampoline);
    }
}  // namespace IWXMVM::IW3::Hooks::PlayerAnimation