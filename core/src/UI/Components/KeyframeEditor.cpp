#include "StdInclude.hpp"
#include "KeyframeEditor.hpp"

#include "UI/UIManager.hpp"
#include "UI/ImGuiExtensions.hpp"
#include "Mod.hpp"
#include "Components/KeyframeManager.hpp"


namespace IWXMVM::UI
{
    void KeyframeEditor::Initialize()
    {
    }

    void KeyframeEditor::DrawKeyframeSlider(const Types::KeyframeableProperty& property)
    {
        auto startTick = 0u;
        auto endTick = Mod::GetGameInterface()->GetDemoInfo().endTick;
        auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;

        ImGuiEx::DrawKeyframeSliderInternal(property, &currentTick, &startTick, &endTick, 
                                            Components::KeyframeManager::Get().GetKeyframes(property));
    }

    void KeyframeEditor::DrawCurveEditor(const Types::KeyframeableProperty& property)
    {
        auto startTick = 0u;
        auto endTick = Mod::GetGameInterface()->GetDemoInfo().endTick;
        auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;

        ImGuiEx::DrawCurveEditorInternal(property, &currentTick, &startTick, &endTick,
                                         Components::KeyframeManager::Get().GetKeyframes(property));
    }

    void KeyframeEditor::Render()
    {
        SetPosition(0, UIManager::Get().GetUIComponent(UI::Component::ControlBar)->GetPosition().y +
                           UIManager::Get().GetUIComponent(UI::Component::ControlBar)->GetSize().y);
        SetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - GetPosition().y);

        ImGui::SetNextWindowPos(GetPosition());
        ImGui::SetNextWindowSize(GetSize());

        const auto padding = ImGui::GetStyle().WindowPadding;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoTitleBar;
        if (ImGui::Begin("Keyframe Editor", nullptr, flags))
        {
            ImGui::SetCursorPos(padding);

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));

            if (ImGui::BeginTable("##keyframeEditorTableLayout", 2, ImGuiTableFlags_SizingFixedFit))
            {
                auto firstColumnSize = ImGui::GetFontSize() * 1.4f + GetSize().x / 8 + padding.x * 3;
            
                ImGui::TableSetupColumn("Properties", ImGuiTableColumnFlags_NoSort, firstColumnSize);
                ImGui::TableSetupColumn("Keyframes", ImGuiTableColumnFlags_NoSort, GetSize().x / 8 * 7);
                ImGui::TableHeadersRow();
            
                for (const auto& pair : Components::KeyframeManager::Get().GetKeyframes())
                {
                    ImGui::TableNextRow();
            
                    ImGui::SetNextItemWidth(GetSize().x / 8);
                    ImGui::TableSetColumnIndex(0);
                    auto showCurve = ImGui::TreeNode(pair.first.name.data());

                    auto progressBarWidth = GetSize().x - firstColumnSize - GetSize().x * 0.05f - padding.x * 2;
                    ImGui::SetNextItemWidth(progressBarWidth);
                    ImGui::TableSetColumnIndex(1);
                    DrawKeyframeSlider(pair.first);

                    if (showCurve)
                    {
                        ImGui::SetNextItemWidth(progressBarWidth);
                        DrawCurveEditor(pair.first);

                        ImGui::TreePop();
                    }
                }
                ImGui::EndTable();
            }

            ImGui::PopStyleVar();

            ImGui::End();
        }
    }

    void KeyframeEditor::Release()
    {
    }
}  // namespace IWXMVM::UI