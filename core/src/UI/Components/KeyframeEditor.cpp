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

    void KeyframeEditor::DrawKeyframeSlider(const Types::KeyframeableProperty& property) const
    {
        auto startTick = 0u;
        auto endTick = Mod::GetGameInterface()->GetDemoInfo().endTick;
        auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;

        ImGuiEx::DrawKeyframeSliderInternal(property, &currentTick, &startTick, &endTick,
                                            Components::KeyframeManager::Get().GetKeyframes(property));
    }

    void DrawCurveEditor(const Types::KeyframeableProperty& property, const auto width)
    {
        auto startTick = 0u;
        auto endTick = Mod::GetGameInterface()->GetDemoInfo().endTick;
        auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;

        // I dont really like the way this is done, I just cant really think of a better solution right now
        auto GetValueCountForProperty = [](auto property) -> int32_t {
            switch (property.valueType)
            {
                case Types::KeyframeValueType::FloatingPoint:
                    return 1;
                case Types::KeyframeValueType::CameraData:
                    return 7;
                case Types::KeyframeValueType::Vector3:
                    return 3;
                default:
                    throw std::runtime_error("Not implemented");
            }
        };
        
        for (int i = 0; i < GetValueCountForProperty(property); i++)
        {
            ImGuiEx::DrawCurveEditorInternal(
                property, &currentTick, &startTick, &endTick, width,
                Components::KeyframeManager::Get().GetKeyframes(property),
                i,
                [i](const auto& keyframe) {
                    return keyframe.value.GetFloatValueByIndex(i);
                },
                [i](auto& keyframe, float value) {
                    keyframe.value.SetFloatValueByIndex(i, value);
                }
            );
        }
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
                    const auto& property = pair.first;
                    const auto& keyframes = pair.second;

                    if (keyframes.empty())
                        continue;

                    ImGui::TableNextRow();

                    ImGui::SetNextItemWidth(GetSize().x / 8);
                    ImGui::TableSetColumnIndex(0);
                    auto showCurve = ImGui::TreeNode(property.name.data());

                    auto progressBarWidth = GetSize().x - firstColumnSize - GetSize().x * 0.05f - padding.x * 2;
                    ImGui::SetNextItemWidth(progressBarWidth);
                    ImGui::TableSetColumnIndex(1);
                    DrawKeyframeSlider(property);

                    if (showCurve)
                    {
                        DrawCurveEditor(property, progressBarWidth);
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
} // namespace IWXMVM::UI
