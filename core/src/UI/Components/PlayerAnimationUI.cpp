#include "StdInclude.hpp"
#include "PlayerAnimationUI.hpp"

#include "Components/PlayerAnimation.hpp"
#include "Mod.hpp"
#include "UI/Components/PrimaryTabs.hpp"
#include <IconsFontAwesome6.h>

namespace IWXMVM::UI
{
void PlayerAnimation::Render()
{
    if (!visible)
        return;

    if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
    {
        ImGui::Begin("Player death animations##1", &visible, ImGuiWindowFlags_NoCollapse);
        UI::DrawInaccessibleTabWarning();
        ImGui::Dummy(ImVec2(450, 0));
        ImGui::End();
        return;
    }

    if (ImGui::Begin("Player death animations##2", &visible, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text(ICON_FA_PERSON_FALLING " (Double) Click to (un)select a death animation   ");
        ImGui::NewLine();
        
        const auto& anims = Components::PlayerAnimation::GetAnimations();
        for (std::int32_t i = 0; i < std::ssize(anims); ++i)
        {
            assert(anims[i].first.length() > 0 && *(anims[i].first.data() + anims[i].first.length()) == '\0');
            static std::int32_t selected = -1;

            if (ImGui::Selectable(anims[i].first.data(), selected == i, ImGuiSelectableFlags_AllowDoubleClick))
            {
                selected = (ImGui::IsMouseDoubleClicked(0)) ? -1 : i;
                Components::PlayerAnimation::SetSelectedAnimIndex(selected);
            }
        }

        ImGui::NewLine();

        if (const auto animName = Components::PlayerAnimation::GetLatestAnimationName(); !animName.empty())
        {
            ImGui::Text("Latest death animation: \n%s", animName.data());
            ImGui::NewLine();
        }

        ImGui::End();
    }
}

void PlayerAnimation::Release()
{
}

void PlayerAnimation::Initialize()
{
}
} // namespace IWXMVM::UI