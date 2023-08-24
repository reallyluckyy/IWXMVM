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
            if (ImGui::BeginMenu("Dollycam")) {
                if (ImGui::MenuItem("Example", "CTRL+D")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About")) {
                if (ImGui::MenuItem("Credits", "")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Open Debug Panel", "CTRL+D")) {}
                ImGui::EndMenu();
            }

            auto windowSize = ImGui::GetIO().DisplaySize;
            std::string iwxmvmText("IWXMVM");
            iwxmvmText += " ";
            iwxmvmText += IWXMVM_VERSION;
            ImGui::SetCursorPosX(windowSize.x - ImGui::CalcTextSize(iwxmvmText.c_str()).x - ImGui::CalcTextSize(" ").x);
            ImGui::Text(iwxmvmText.c_str());

            ImGui::EndMainMenuBar();
        }
    }

    void MenuBar::Release()
    {}
}