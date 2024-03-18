#include "StdInclude.hpp"
#include "PlayerAnimationUI.hpp"

#include "Components/PlayerAnimation.hpp"
#include "IconsFontAwesome6.h"
#include "Mod.hpp"
#include "UI/Components/PrimaryTabs.hpp"
#include "UI/UIManager.hpp"

namespace IWXMVM::UI
{
bool DrawHeaderAndResetButton(const char* label)
{
    ImGui::AlignTextToFramePadding();
    ImGui::PushFont(UIManager::Get().GetBoldFont());
    ImGui::Text(label);
    ImGui::PopFont();
    ImGui::SameLine();

    const char* resetlabel = ICON_FA_REPEAT " Reset ";
    const bool ret = ImGui::Button(resetlabel);
    return ret;
}

void PlayerAnimation::Render()
{
    if (!visible)
        return;

    ImGui::SetNextWindowFocus();

    if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
    {
        ImGui::Begin("Player Death Animations##1", &visible, ImGuiWindowFlags_NoCollapse);
        UI::DrawInaccessibleTabWarning();
        ImGui::Dummy(ImVec2(450, 0));
        ImGui::End();
        return;
    }

    if (ImGui::Begin("Player Death Animations##2", &visible, ImGuiWindowFlags_NoCollapse))
    {
        static std::int32_t selected = -1;
        if (DrawHeaderAndResetButton(ICON_FA_PERSON_FALLING "  Select a death animation   "))
            Components::PlayerAnimation::SetSelectedAnimIndex(selected = -1);
        
        const auto& anims = Components::PlayerAnimation::GetAnimations();
        for (std::int32_t i = 0; i < std::ssize(anims); ++i)
        {
            assert(anims[i].first.length() > 0 && *(anims[i].first.data() + anims[i].first.length()) == '\0');

            if (ImGui::Selectable(anims[i].first.data(), selected == i))
                Components::PlayerAnimation::SetSelectedAnimIndex(selected = i);
        }

        ImGui::NewLine();

        if (const auto animName = Components::PlayerAnimation::GetLatestAnimationName(); !animName.empty())
            ImGui::Text("Latest death animation: \n%s", animName.data());
        else 
        {
            ImGui::NewLine();
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