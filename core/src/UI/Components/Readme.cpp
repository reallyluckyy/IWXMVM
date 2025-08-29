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

    bool visible = true;

    void Readme::Render()
    {
        ImGui::SetNextWindowSize(ImVec2(1400, 800));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 20));

        if (ImGui::BeginPopupModal("Readme", &visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
        {

            ImGui::PushFont(UIManager::Get().GetBoldFont()); 
            ImGui::Text("Hello!");
            ImGui::PopFont();

            ImGui::Dummy(ImVec2(0, 10));

            auto indent = 10;

            ImGui::TextWrapped("Welcome to IWXMVM, a recording mod for various Call of Duty games!");
            ImGui::TextWrapped("While this is an official release, there will still be bugs. If you find any, feel free to open an issue on https://github.com/reallyluckyy/IWXMVM/.");

            ImGui::Dummy(ImVec2(0, 20));

            ImGui::PushFont(UIManager::Get().GetBoldFont());
            ImGui::TextWrapped(ICON_FA_BELL "  WHATS NEW");
            ImGui::PopFont();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped("Multipass Recording! Navigate to the " ICON_FA_CIRCLE "  RECORD tab to select from multiple options, such as greenscreened footage, depth or even normals.");

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::PushFont(UIManager::Get().GetBoldFont()); 
            ImGui::TextWrapped(ICON_FA_SIGNS_POST "  THE INTERFACE");
            ImGui::PopFont();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped("The UI is divided into three sections.");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped(
                "The game view in the top left shows the game. The bottom area will contain the playback controls and the keyframe editor as soon as you load a demo. To the right of the game view is the main settings section, consisting of 4 tabs:");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::BulletText(ICON_FA_FILE "  DEMOS: for loading demos");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::BulletText(ICON_FA_VIDEO "  CAMERA: for camera related settings, such as fov");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::BulletText(ICON_FA_SLIDERS "  VISUALS: for modifying settings such as filmtweaks, sun, dof, hud, etc.");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::BulletText(ICON_FA_CIRCLE "  RECORD: for exporting videos right from within the mod");

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::PushFont(UIManager::Get().GetBoldFont()); 
            ImGui::TextWrapped(ICON_FA_COMPASS "  GETTING STARTED");
            ImGui::PopFont();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped("On the right, select the first tab (" ICON_FA_FILE " DEMOS) and browse for a demo that you want to watch.");
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
            ImGui::TextWrapped(
                "Once thats loaded up you should have the demo controls, the keyframe editor, the tabs on the right as "
                "well as the camera controls available to you.");

            if (D3D9::IsReshadePresent()) 
            {
                ImGui::Dummy(ImVec2(0, 10));

                ImGui::PushFont(UIManager::Get().GetBoldFont());
                ImGui::TextWrapped(ICON_FA_TRIANGLE_EXCLAMATION "  USING RESHADE");
                ImGui::PopFont();

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
                ImGui::TextWrapped(
                    "To interact with Reshade it is recommended you turn off the UI first by pressing F1."
                    "\nAlternatively, you may click on Debug -> Toggle IWXMVM UI.");
            }
            
            ImGui::Dummy(ImVec2(0, 20));

            ImGui::End();
        }

        ImGui::PopStyleVar();
    }

    void Readme::Release()
    {
    }
}  // namespace IWXMVM::UI