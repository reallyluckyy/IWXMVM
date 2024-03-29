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
                    UIManager::Get().GetUIComponent<Preferences>(Component::Preferences)->ToggleVisibility();
                }
                if (ImGui::MenuItem("Controls"))
                {
                    UIManager::Get().GetUIComponent<ControlsMenu>(Component::ControlsMenu)->ToggleVisibility();
                }
                if (ImGui::MenuItem("Exit"))
                {
                    Mod::RequestEject();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Debug"))
            {
                if (ImGui::MenuItem("Toggle IWXMVM UI", "F1"))
                {
                    UIManager::Get().ToggleOverlay();
                }

                if (ImGui::MenuItem("Toggle ImGui Demo", "F3"))
                {
                    UIManager::Get().ToggleImGuiDemo();
                }

                if (ImGui::MenuItem("Toggle Debug Panel", "F4"))
                {
                    UIManager::Get().ToggleDebugPanel();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools"))
            {
                ImGui::BeginDisabled(
                    !(Mod::GetGameInterface()->GetSupportedFeatures() & Types::Features_ChangeAnimations)
                );
                
                if (ImGui::MenuItem("Player Death Animations##0"))
                {
                    UIManager::Get().GetUIComponent<PlayerAnimation>(Component::PlayerAnimation)->ToggleVisibility();
                }

                ImGui::EndDisabled();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About"))
            {
                if (ImGui::MenuItem("Credits", ""))
                {
                    UIManager::Get().GetUIComponent<Credits>(Component::Credits)->ToggleVisibility();
                }
                ImGui::EndMenu();
            }

            auto windowSize = ImGui::GetIO().DisplaySize;
            std::string iwxmvmText = std::format(
                "IWXMVM {0} | {1}", IWXMVM_VERSION, magic_enum::enum_name(Mod::GetGameInterface()->GetGame())
            );
            ImGui::SetCursorPosX(windowSize.x - ImGui::CalcTextSize(iwxmvmText.c_str()).x - ImGui::CalcTextSize(" ").x);
            ImGui::Text(iwxmvmText.c_str());

            ImGui::EndMainMenuBar();
        }
    }

    void MenuBar::Release()
    {
    }
}  // namespace IWXMVM::UI