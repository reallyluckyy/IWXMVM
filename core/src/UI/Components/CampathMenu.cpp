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

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Interpolation: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (ImGui::BeginCombo("##campathMenuInterpolationCombo",
                              campathManager.GetInterpolationModeLabel(campathManager.GetInterpolationMode()).data()))
        {
            for (auto interpolationMode : campathManager.GetInterpolationModes())
            {
                bool isSelected = campathManager.GetInterpolationMode() == interpolationMode;
                if (ImGui::Selectable(campathManager.GetInterpolationModeLabel(interpolationMode).data(),
                                      campathManager.GetInterpolationMode() == interpolationMode))
                {
                    campathManager.SetInterpolationMode(interpolationMode);
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

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
        /*
        constexpr auto NODE_COLOR = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
        constexpr auto NODE_SIZE = ImVec2(10, 10);
        constexpr auto PATH_COLOR = ImVec4(1.0f, 0.75f, 0.0f, 0.8f);

        const auto& campathManager = Components::CampathManager::Get();

        for (const auto& node : campathManager.GetNodes())
        {
            ImGuiEx::DrawPoint3D(node.position, NODE_COLOR, NODE_SIZE);
            ImGuiEx::DrawLine3D(node.position, node.position + MathUtils::ForwardVectorFromAngles(node.rotation) * 50,
                                NODE_COLOR, NODE_SIZE.x / 2);
        }

        if (campathManager.GetNodes().size() >= campathManager.GetRequiredNodeCount())
        {
            const auto EVALUATION_DISTANCE = glm::clamp(
                100 * (campathManager.GetNodes().back().tick - campathManager.GetNodes().front().tick) / 5000, 1u,
                1000u);

            auto previousNode = campathManager.GetNodes().front();
            for (auto tick = campathManager.GetNodes().front().tick + EVALUATION_DISTANCE;
                 tick <= campathManager.GetNodes().back().tick; tick += EVALUATION_DISTANCE)
            {
                auto node = Components::CampathManager::Get().Interpolate(tick);
                ImGuiEx::DrawLine3D(previousNode.position, node.position, PATH_COLOR, NODE_SIZE.x / 2);

                previousNode = node;
            }
            ImGuiEx::DrawLine3D(previousNode.position, campathManager.GetNodes().back().position, PATH_COLOR,
                                NODE_SIZE.x / 2);
        }
        */
    }

    void CampathMenu::Release()
    {
    }
}  // namespace IWXMVM::UI