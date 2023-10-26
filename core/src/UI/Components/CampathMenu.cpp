#include "StdInclude.hpp"
#include "CampathMenu.hpp"

#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "UI/ImGuiExtensions.hpp"
#include "Components/CameraManager.hpp"
#include "Input.hpp"
#include "Events.hpp"
#include "Utilities/MathUtils.hpp"

namespace IWXMVM::UI
{
    void CampathMenu::Initialize()
    {
        Events::RegisterListener(EventType::OnRenderGameView, [&]() {
            if (Components::CameraManager::Get().GetActiveCamera()->GetMode() != Components::Camera::Mode::Dolly)
            {
                DrawCampath();
            }
        });
    }

    void CampathMenu::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("Campath", NULL, flags);

        auto& campathManager = Components::CampathManager::Get();

        ImGui::End();

        if (Mod::GetGameInterface()->GetGameState() == Types::GameState::InDemo &&
            Components::CameraManager::Get().GetActiveCamera()->GetMode() != Components::Camera::Mode::Dolly)
        {
            RenderHotkeyOverlay();
        }
    }

    void DrawKeyLine(Action action, std::string_view label, ImVec2 position)
    {
        auto text = std::format("{} - {}", ImGui::GetKeyName(InputConfiguration::Get().GetBoundKey(action)), label);
        ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), position,
                                                ImGui::GetColorU32(ImGuiCol_Text), text.data());
    }

    void CampathMenu::RenderHotkeyOverlay()
    {
        // Drawing things "directly" since there doesnt seem to be a way to create a window thats always on top...

        auto overlaySize = ImVec2(ImGui::GetFontSize() * 14, ImGui::GetFontSize() * 7);
        auto overlayMargin = ImVec2(ImGui::GetFontSize() * 1, ImGui::GetFontSize() * 1);
        auto overlayPadding = ImVec2(ImGui::GetFontSize() * 0.75f, ImGui::GetFontSize() * 0.75f);
        auto lineHeight = 1.15f * ImGui::GetFontSize();

        auto& gameView = UIManager::Get().GetUIComponent(Component::GameView);
        auto basePos = gameView->GetPosition() + gameView->GetSize() - overlaySize - overlayMargin;
        ImGui::GetForegroundDrawList()->AddRectFilled(basePos, basePos + overlaySize,
                                                      ImGui::GetColorU32(ImGuiCol_WindowBg));
        ImGui::GetForegroundDrawList()->AddRect(basePos, basePos + overlaySize, ImGui::GetColorU32(ImGuiCol_Border));
        auto contentBasePos = basePos + overlayPadding;

        ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), contentBasePos,
                                                ImGui::GetColorU32(ImGuiCol_Text), "Campath Hotkeys");

        DrawKeyLine(Action::DollyAddNode, "Add Node",
                    contentBasePos + ImVec2(0, lineHeight * 1.5));
        DrawKeyLine(Action::DollyPlayPath,
                    "Switch To Dollycam", contentBasePos + ImVec2(0, lineHeight * 1.5 + lineHeight));
        DrawKeyLine(Action::DollyClearNodes,
                    "Delete All Nodes", contentBasePos + ImVec2(0, lineHeight * 1.5 + lineHeight * 2));
    }

    void CampathMenu::DrawCampath()
    {
        constexpr auto NODE_COLOR = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
        constexpr auto NODE_SIZE = ImVec2(10, 10);
        constexpr auto PATH_COLOR = ImVec4(1.0f, 0.75f, 0.0f, 0.8f);

        auto& keyframeManager = Components::KeyframeManager::Get();
        const auto& property = keyframeManager.GetProperty(Types::KeyframeablePropertyType::CampathCamera);
        auto nodes = keyframeManager.GetKeyframes(property);

        for (const auto& node : nodes)
        {
            ImGuiEx::DrawPoint3D(node.value.cameraData.position, NODE_COLOR, NODE_SIZE);
            ImGuiEx::DrawLine3D(node.value.cameraData.position,
                                node.value.cameraData.position + MathUtils::ForwardVectorFromAngles(node.value.cameraData.rotation) * 50,
                                NODE_COLOR, NODE_SIZE.x / 2);
        }
        
        if (nodes.size() >= 1)
        {
            const auto EVALUATION_DISTANCE =
                glm::clamp(100 * (nodes.back().tick - nodes.front().tick) / 5000, 1u,
                1000u);
        
            auto previousNode = nodes.front();
            for (auto tick = nodes.front().tick + EVALUATION_DISTANCE; tick <= nodes.back().tick;
                 tick += EVALUATION_DISTANCE)
            {
                auto node = keyframeManager.Interpolate(property, tick);
                ImGuiEx::DrawLine3D(previousNode.value.cameraData.position, node.value.cameraData.position, PATH_COLOR, NODE_SIZE.x / 2);
        
                previousNode = node;
            }
            ImGuiEx::DrawLine3D(previousNode.value.cameraData.position, nodes.back().value.cameraData.position,
                                PATH_COLOR,
                                NODE_SIZE.x / 2);
        }
    }

    void CampathMenu::Release()
    {
    }
}  // namespace IWXMVM::UI