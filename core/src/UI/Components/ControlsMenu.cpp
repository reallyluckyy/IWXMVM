#include "StdInclude.hpp"
#include "ControlsMenu.hpp"

#include "Configuration/InputConfiguration.hpp"

namespace IWXMVM::UI
{
    void ControlsMenu::Initialize()
    {
        SetPosition(0, 0);
        SetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetFrameHeight());
    }

    Action selectedAction = Action::Count;

    const char* MODAL_TITLE = "Select Key##controlsMenuKeybindPopup";

    void DrawKeybindRow(Action action)
    {
        auto actionName = InputConfiguration::Get().ActionToString(action);
        auto key = InputConfiguration::Get().GetBoundKey(action);
        auto keyName = ImGui::GetKeyName(key);

        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", actionName.data());

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button(std::format("{0}##{1}", keyName, (int)action).c_str(),
                          ImVec2(170, 0)))
        {
            ImGui::OpenPopup(MODAL_TITLE);
            selectedAction = static_cast<Action>(action);
        } 
    }

    void DrawPopup()
    {
        if (ImGui::BeginPopupModal(MODAL_TITLE))
        {
            ImGui::Text("Press a key to bind to %s\nESC to cancel",
                        InputConfiguration::Get().ActionToString(selectedAction).data());
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
            {
                ImGui::CloseCurrentPopup();
            }
            else
            {
                for (auto key = 0; key < ImGuiKey_COUNT; key++)
                {
                    if (ImGui::IsKeyPressed((ImGuiKey)key))
                    {
                        InputConfiguration::Get().SetKeyBinding(selectedAction, static_cast<ImGuiKey>(key));
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            ImGui::EndPopup();
        }
    }

    void ControlsMenu::Render()
    {
        if (!visible)
            return;

        if (ImGui::Begin("Controls", &visible, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
        {
            if (ImGui::BeginTable("##controlsMenuTableOuter", 2, ImGuiTableFlags_SizingStretchSame))
            {
                ImGui::TableNextColumn();
                if (ImGui::BeginTable("##controlsMenuTableInner1", 2))
                {
                    for (auto action = 0; action < Action::PlaybackFaster; action++)
                    {
                        ImGui::TableNextRow();
                        DrawKeybindRow(static_cast<Action>(action));
                    }

                    DrawPopup();

                    ImGui::EndTable();
                }

                ImGui::TableNextColumn();
                if (ImGui::BeginTable("##controlsMenuTableInner2", 2))
                {
                    for (auto action = (int)Action::PlaybackFaster; action < Action::Count; action++)
                    {
                        ImGui::TableNextRow();
                        DrawKeybindRow(static_cast<Action>(action));
                    }

                    DrawPopup();

                    ImGui::EndTable();
                }

                ImGui::EndTable();
            }

            ImGui::End();
        }
    }

    void ControlsMenu::Release()
    {
    }
}  // namespace IWXMVM::UI