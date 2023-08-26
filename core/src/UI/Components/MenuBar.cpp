#include "StdInclude.hpp"
#include "MenuBar.hpp"

#include "Version.hpp"
#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM::UI
{
    void MenuBar::Initialize()
    {
        SetPosition(0, 0);
        SetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetFrameHeight());
    }

    void MenuBar::Render()
    {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Preferences")) {}
                if (ImGui::MenuItem("Edit Controls")) {}
                if (ImGui::MenuItem("Exit")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About")) {
                if (ImGui::MenuItem("Credits", "")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Open Debug Panel", "CTRL+D")) {}
                ImGui::EndMenu();
            }

            auto windowSize = ImGui::GetIO().DisplaySize;
            std::string iwxmvmText = std::format("IWXMVM {0} | {1}", IWXMVM_VERSION, Mod::GetGameInterface()->GetGameName());
            ImGui::SetCursorPosX(windowSize.x - ImGui::CalcTextSize(iwxmvmText.c_str()).x - ImGui::CalcTextSize(" ").x);
            ImGui::Text(iwxmvmText.c_str());

            ImGui::EndMainMenuBar();
        }
    }

    void MenuBar::Release()
    {}
}