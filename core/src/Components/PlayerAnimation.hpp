#pragma once

namespace IWXMVM::Components::PlayerAnimation
{
    void SetPlayerAnimation(std::string_view curAnimName, std::uint32_t& newAnimIndex);

    void PopulateAnimationData(std::vector<std::pair<std::string_view, std::uint32_t>>& anims);
    std::string_view GetLatestAnimationName();
    const std::vector<std::pair<std::string_view, std::uint32_t>>& GetAnimations();
    void SetSelectedAnimIndex(std::int32_t index);
    bool& HideAllCorpses();
    bool& AttachWeaponToCorpse();
}  // namespace IWXMVM::Components::PlayerAnimation