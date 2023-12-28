#include "StdInclude.hpp"
#include "MenuBar.hpp"

#include "Version.hpp"
#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"
#include "UI/UIManager.hpp"

namespace IWXMVM::UI
{
    void MenuBar::Initialize()
    {
        SetPosition(0, 0);
        SetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetFrameHeight());
    }

    void MenuBar::Render()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Preferences"))
                {
                }
                if (ImGui::MenuItem("Controls"))
                {
                    UIManager::Get().GetUIComponent<ControlsMenu>(Component::ControlsMenu)->ToggleVisibility();
                }
                if (ImGui::MenuItem("Exit"))
                {
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Debug"))
            {
                if (ImGui::MenuItem("Toggle IWXMVM UI", "F1"))
                {
                    UIManager::Get().ToggleOverlay();
                }

                if (ImGui::MenuItem("Toggle ImGui Demo", "F2"))
                {
                    UIManager::Get().ToggleImGuiDemo();
                }

                if (ImGui::MenuItem("Toggle Debug Panel", "F3"))
                {
                    UIManager::Get().ToggleDebugPanel();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About"))
            {
                if (ImGui::MenuItem("Credits", ""))
                {
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Open Debug Panel", "CTRL+D"))
                {
                }
                ImGui::EndMenu();
            }

            auto windowSize = ImGui::GetIO().DisplaySize;
            std::string iwxmvmText =
                std::format("IWXMVM {0} | {1}", IWXMVM_VERSION, Types::ToString(Mod::GetGameInterface()->GetGame()));
            ImGui::SetCursorPosX(windowSize.x - ImGui::CalcTextSize(iwxmvmText.c_str()).x - ImGui::CalcTextSize(" ").x);
            ImGui::Text(iwxmvmText.c_str());

            ImGui::EndMainMenuBar();
        }
    }

    void MenuBar::Release()
    {
    }
}  // namespace IWXMVM::UI