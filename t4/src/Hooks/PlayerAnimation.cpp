#include "StdInclude.hpp"
#include "PlayerAnimation.hpp"

#include "../Addresses.hpp"
#include "../Structures.hpp"
#include "Components/PlayerAnimation.hpp"
#include "Mod.hpp"
#include "Utilities/HookManager.hpp"

namespace IWXMVM::T4::Hooks::PlayerAnimation
{    
    inline constexpr std::size_t ANIM_COUNT = 318;

    bool GetAnimationNames()
    {
        // TODO: when bgs is figured out
        /*
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
        */
        LOG_ERROR("Getting death animation names was unsuccessful!");
        return false;
    }

    void CG_ProcessEntity_Hook_Internal(Structures::centity_s& centity)
    {
        using namespace Structures;

        static bool foundDeathAnimations = GetAnimationNames();
        static std::array<std::uint8_t, 64> weaponIndices;

        // TODO: const auto& ps = Structures::GetClientGlobals()->predictedPlayerState;
        // TODO: if (static_cast<std::size_t>(ps.clientNum) < weaponIndices.size() && ps.weapon != 0)
        // TODO: {
        // TODO:     weaponIndices[ps.clientNum] = ps.weapon;
        // TODO: }

        if (centity.nextState.eType == entityType_t::ET_PLAYER)
        {
            if (static_cast<std::size_t>(centity.nextState.clientNum) < weaponIndices.size() &&
                centity.nextState.weapon != 0)
            {
                weaponIndices[centity.nextState.clientNum] = centity.nextState.weapon;
            }

            auto IsBadIndex = [](auto index)
            {
                static constexpr std::array animIndexRanges{std::array{253, 260}, std::array{273, 280}};

                return std::any_of(animIndexRanges.begin(), animIndexRanges.end(), [index](const auto range) {
                    return index >= range.front() && index <= range.back();
                });
            };

            auto& torsoAnim = centity.nextState.torsoAnim;
            if (IsBadIndex(torsoAnim & 511))
            {
                // promod animation bugs fix
                torsoAnim = 0;
            }
        }
        else if (centity.nextState.eType == entityType_t::ET_PLAYER_CORPSE)
        {
            // TODO: auto& legsAnim = reinterpret_cast<std::uint32_t&>(centity.nextState.legsAnim);
            // TODO: 
            // TODO: if ((legsAnim & 511) != 0 && foundDeathAnimations)
            // TODO: {
            // TODO:     const auto animations = std::span{GetClientGlobals()->bgs.animScriptData.animations};
            // TODO: 
            // TODO:     if ((legsAnim & 511) < std::min(ANIM_COUNT, animations.size()))
            // TODO:     {
            // TODO:         const std::string_view animName{animations[(legsAnim & 511)].name};
            // TODO:         if (animName.find("death") != std::string_view::npos)
            // TODO:         {
            // TODO:             // replace death animation
            // TODO:             Components::PlayerAnimation::SetPlayerAnimation(animName, legsAnim);
            // TODO:         } 
            // TODO:     }
            // TODO: }

            if (Components::PlayerAnimation::AttachWeaponToCorpse()) 
            {
                if (static_cast<std::size_t>(centity.nextState.clientNum) < weaponIndices.size())
                {
                    // attach weapon to corpse
                    centity.nextState.weapon = weaponIndices[centity.nextState.clientNum];
                }
            }
            else
            {
                // detach any previously attached weapon from corpse
                centity.nextState.weapon = 0;
            }

            if (Components::PlayerAnimation::HideAllCorpses())
            {
                // hide corpses
                // TODO: centity.nextState.lerp.eFlags |= 32;
            }
            else
            {
                // reveal corpses
               // TODO: centity.nextState.lerp.eFlags &= ~32;
            }
        }
        else if (centity.nextState.eType == entityType_t::ET_ITEM)
        {
            if (Components::PlayerAnimation::AttachWeaponToCorpse())
            {
                // hide dropped weapons
                // TODO: centity.nextState.lerp.eFlags |= 32;
            }
            else
            {
                // reveal dropped weapons
                // TODO: centity.nextState.lerp.eFlags &= ~32;
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
        // TODO: 
        // HookManager::CreateHook(GetGameAddresses().CG_ProcessEntity(), (std::uintptr_t)CG_ProcessEntity_Hook,
        //                        &CG_ProcessEntity_Trampoline);
    }
}  // namespace IWXMVM::T4::Hooks::PlayerAnimation
