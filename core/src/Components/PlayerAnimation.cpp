#include "StdInclude.hpp"
#include "PlayerAnimation.hpp"

namespace IWXMVM::Components::PlayerAnimation
{
    std::string_view latestAnimName;
    std::int32_t selectedAnimIndex = -1;
    std::vector<std::pair<std::string_view, std::uint32_t>> cachedAnims;

    void PopulateAnimationData(std::vector<std::pair<std::string_view, std::uint32_t>>& anims)
    {
        cachedAnims = std::move(anims);
    }

    const std::vector<std::pair<std::string_view, std::uint32_t>>& GetAnimations()
    {
        return cachedAnims;
    }

    std::string_view GetLatestAnimationName()
    {
        return latestAnimName;
    }

    void SetSelectedAnimIndex(std::int32_t index)
    {
        selectedAnimIndex = index;
    }

    void SetPlayerAnimation(std::string_view animName, std::uint32_t& animIndex)
    {
        latestAnimName = animName;

        if (static_cast<std::uint32_t>(selectedAnimIndex) < cachedAnims.size()) 
            animIndex = cachedAnims[static_cast<std::uint32_t>(selectedAnimIndex)].second;
    }
}  // namespace IWXMVM::Components::PlayerAnimation