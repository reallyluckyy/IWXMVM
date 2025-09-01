#include "StdInclude.hpp"
#include "MiscellaneousSettings.hpp"

#include "Mod.hpp"
#include "UI/Components/PrimaryTabs.hpp"

namespace IWXMVM::UI
{
    void MiscellaneousSettings::Render()
    {
        if (!visible)
            return;

        ImGui::SetNextWindowFocus();

        if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
        {
            ImGui::Begin("Miscellaneous Settings##1", &visible, ImGuiWindowFlags_NoCollapse);
            UI::DrawInaccessibleTabWarning();
            ImGui::Dummy(ImVec2(450, 0));
            ImGui::End();
            return;
        }

        if (ImGui::Begin("Miscellaneous Settings##2", &visible, ImGuiWindowFlags_NoCollapse))
        {
            bool button = Mod::GetGameInterface()->Disable_DemoCvars(false);

            ImGui::NewLine();
            if (ImGui::Checkbox("Disable Demo Cvars", &button))
            {
                Mod::GetGameInterface()->Disable_DemoCvars(true);
            }
            ImGui::NewLine();

            ImGui::End();
        }
    }

    void MiscellaneousSettings::Release()
    {
    }

    void MiscellaneousSettings::Initialize()
    {
    }
} // namespace IWXMVM::UI
