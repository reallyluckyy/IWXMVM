#include "StdInclude.hpp"
#include "MenuBar.hpp"

#include "Version.hpp"
#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"

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

            auto windowSize = PathUtils::GetWindowSize(Mod::GetGameInterface()->GetWindowHandle());
            ImGui::SetCursorPosX(windowSize.x - 90);
            ImGui::Text("IWXMVM %s", IWXMVM_VERSION);

            ImGui::EndMainMenuBar();
        }
    }
}
