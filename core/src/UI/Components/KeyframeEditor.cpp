#include "StdInclude.hpp"
#include "KeyframeEditor.hpp"

#include "UI/UIManager.hpp"
#include "UI/ImGuiExtensions.hpp"
#include "Mod.hpp"


namespace IWXMVM::UI
{
    void KeyframeEditor::Initialize()
    {
    }

    void KeyframeEditor::DrawKeyframeSlider(const char* label)
    {
        auto startTick = 0u;
        auto endTick = Mod::GetGameInterface()->GetDemoInfo().endTick;
        auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;

        std::vector<Types::Keyframe> keyframes;
        for (const auto& node : Components::CampathManager::Get().GetNodes())
        {
            keyframes.push_back(Types::Keyframe((std::uint32_t*)&node.tick, (float*)&node.position[0]));
        }
        ImGuiEx::DrawKeyframeSliderInternal(label, &currentTick, &startTick, &endTick, keyframes);
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
            
                for (int row = 0; row < 4; row++)
                {
                    ImGui::TableNextRow();
            
                    ImGui::SetNextItemWidth(GetSize().x / 8);
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::TreeNode("Test##1"))
                    {
                        ImGui::TreePop();
                    }
            
                    auto progressBarWidth = GetSize().x - firstColumnSize - GetSize().x * 0.05f - padding.x;
                    ImGui::SetNextItemWidth(progressBarWidth);
                    ImGui::TableSetColumnIndex(1);
                    DrawKeyframeSlider("Test2##1");
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