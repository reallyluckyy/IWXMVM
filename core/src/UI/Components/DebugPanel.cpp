#include "StdInclude.hpp"
#include "DebugPanel.hpp"

#include "Utilities/HookManager.hpp"
#include "UI/UIManager.hpp"
#include "Mod.hpp"

namespace IWXMVM::UI
{
    void DebugPanel::Initialize()
    {
    }

    void DebugPanel::Render()
    {
        if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            const auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();

            ImGui::Text("Game State: %s", Types::ToString(Mod::GetGameInterface()->GetGameState()).data());
            ImGui::Text("Demo Name: %s", demoInfo.name.c_str());
            if (demoInfo.frozenTick.has_value())
            {
                ImGui::Text("Frozen Demo Tick: %d", demoInfo.frozenTick.value());
            }
            ImGui::Text("Demo Tick: %d", demoInfo.currentTick);
            ImGui::Text("Demo End Tick: %d", demoInfo.endTick);

            auto keyframeEditor = UIManager::Get().GetUIComponent<KeyframeEditor>(UI::Component::KeyframeEditor);
            auto [displayStartTick, displayEndTick] = keyframeEditor->GetDisplayTickRange();
            ImGui::Text("KE Tick Range: %d to %d", displayStartTick, displayEndTick);

            auto& camera = Components::CameraManager::Get().GetActiveCamera();
            ImGui::Text("Camera: %f %f %f", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z);
            if (ImGui::Button("Eject"))
                Mod::RequestEject();
            ImGui::End();
        }
    }

    void DebugPanel::Release()
    {
    }
}  // namespace IWXMVM::UI