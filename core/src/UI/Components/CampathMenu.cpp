#include "StdInclude.hpp"
#include "CampathMenu.hpp"

#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "UI/ImGuiEx/ImGuiExtensions.hpp"
#include "Components/CameraManager.hpp"
#include "Input.hpp"
#include "Events.hpp"
#include "Utilities/MathUtils.hpp"

namespace IWXMVM::UI
{
    void CampathMenu::Initialize()
    {
    }

    void CampathMenu::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("Campath", NULL, flags);

        if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
        {
            UI::DrawInaccessibleTabWarning();
            ImGui::End();
            return;
        }

        auto& campathManager = Components::CampathManager::Get();

        if (Components::CameraManager::Get().GetActiveCamera()->GetMode() != Components::Camera::Mode::Dolly)
        {
            ImGui::Text("Campath Controls");
            ImGui::Text("%s - Add Node",
                        ImGui::GetKeyName(InputConfiguration::Get().GetBoundKey(Action::DollyAddNode)));
            ImGui::Text("%s - Clear All Nodes",
                        ImGui::GetKeyName(InputConfiguration::Get().GetBoundKey(Action::DollyClearNodes)));
            ImGui::Text("%s - Switch To Dollycam",
                        ImGui::GetKeyName(InputConfiguration::Get().GetBoundKey(Action::DollyPlayPath)));
        }

        ImGui::End();
    }

    void CampathMenu::Release()
    {
    }
}  // namespace IWXMVM::UI