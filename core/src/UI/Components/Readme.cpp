#include "StdInclude.hpp"
#include "Readme.hpp"

#include "UI/UIManager.hpp"
#include "Version.hpp"
#include "Mod.hpp"
#include "Resources.hpp"

namespace IWXMVM::UI
{
    void Readme::Initialize()
    {
        ImGui::OpenPopup("Readme");
    }

    void Readme::Render()
    {
        ImGui::SetNextWindowSize(ImVec2(1400, 800));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 20));

        if (ImGui::BeginPopupModal("Readme", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
        {

            ImGui::PushFont(UIManager::Get().GetBoldFont()); 
            ImGui::Text("IWXMVM BETA");
            ImGui::PopFont();

            ImGui::Dummy(ImVec2(0, 20));

            auto indent = 10;

            ImGui::TextWrapped("First off, thank you for testing and providing feedback on this mod!");
            ImGui::TextWrapped(
                "Hearing how you use the mod, what kind of issues you experience and where you expected a better "
                "experience will help us make IWXMVM the best it can be.");

            ImGui::Dummy(ImVec2(0, 20));

            ImGui::PushFont(UIManager::Get().GetBoldFont()); 
            ImGui::TextWrapped(ICON_FA_SIGNS_POST " THE INTERFACE");
            ImGui::PopFont();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped("The UI is divided into three sections.");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped(
                "The game view in the top left shows the game. The bottom area will contain the playback controls and the keyframe editor as soon as you load a demo. To the right of the game view is the main settings section, consisting of 4 tabs:");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::BulletText(ICON_FA_FILE " DEMOS: for loading demos");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::BulletText(ICON_FA_VIDEO "  CAMERA: for camera related settings; currently mostly a placeholder");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::BulletText(ICON_FA_SLIDERS " VISUALS: for modifying settings such as filmtweaks, sun, dof, hud, etc.");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::BulletText(ICON_FA_CIRCLE " RECORD: for exporting videos right from within the mod");

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::PushFont(UIManager::Get().GetBoldFont()); 
            ImGui::TextWrapped(ICON_FA_COMPASS " GETTING STARTED");
            ImGui::PopFont();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped("On the right, select the first tab (" ICON_FA_FILE " DEMOS) and browse for a demo that you want to watch.");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped(
                "Once thats loaded up you should have the demo controls, the keyframe editor, the tabs on the right as "
                "well as the camera controls available to you.");

            
            ImGui::Dummy(ImVec2(0, 20));

            ImGui::PushFont(UIManager::Get().GetBoldFont());
            ImGui::TextWrapped(ICON_FA_CLIPBOARD " FINAL NOTES");
            ImGui::PopFont();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped("Try everything! Press every button you can find!");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped("Try to follow your recording workflow and write down whatever is confusing to you or what you're missing from the current set of features.");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped("Please report anything and everything so we can get an overview over what works and what doesnt.");
            
            ImGui::Dummy(ImVec2(0, 20));
            
            ImGui::TextWrapped("Thank you!");

            ImGui::End();
        }

        ImGui::PopStyleVar();
    }

    void Readme::Release()
    {
    }
}  // namespace IWXMVM::UI