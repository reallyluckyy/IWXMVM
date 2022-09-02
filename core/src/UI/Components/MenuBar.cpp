#include "StdInclude.hpp"
#include "MenuBar.hpp"

namespace IWXMVM::UI
{

    void MenuBar::Initialize()
    {

    }

    void MenuBar::Render()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Dollycam"))
            {
                if (ImGui::MenuItem("Example", "CTRL+D")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About"))
            {
                if (ImGui::MenuItem("Credits", "")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Open Debug Panel", "CTRL+D")) {}
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }
}