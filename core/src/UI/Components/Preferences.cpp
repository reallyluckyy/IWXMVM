#include "StdInclude.hpp"
#include "Preferences.hpp"

#include "Configuration/PreferencesConfiguration.hpp"

namespace IWXMVM::UI
{
    void Preferences::Initialize()
    {
    }

    void DrawHeading(const char* label)
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::Text(label);
        ImGui::PopFont();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
    }

    void DrawMiscSection()
    {
        auto& preferences = PreferencesConfiguration::Get();

        DrawHeading("General");
        ImGui::Checkbox("Show Keybind Hints in Game View", &preferences.showKeybindHints);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    }

    void DrawFreecamSection()
    {
        auto& preferences = PreferencesConfiguration::Get();

        DrawHeading("Freecam");
        ImGui::DragFloat("Movement Speed", &preferences.freecamSpeed, 1.0f, 1.0f, 500.0f, "%.0f");
        ImGui::DragFloat("Mouse Sensitivity", &preferences.freecamMouseSpeed, 0.01f, 0.01f, 5.0f, "%.2f");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    }

    void DrawOrbitCameraSection()
    {
        auto& preferences = PreferencesConfiguration::Get();

        DrawHeading("Orbit Camera");
        ImGui::DragFloat("Rotation Speed", &preferences.orbitRotationSpeed, 0.01f, 0.01f, 5.0f, "%.2f");
        ImGui::DragFloat("Movement Speed", &preferences.orbitMoveSpeed, 0.01f, 0.01f, 5.0f, "%.2f");
        ImGui::DragFloat("Zoom Speed", &preferences.orbitZoomSpeed, 0.01f, 0.01f, 5.0f, "%.2f");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    }

    void Preferences::Render()
    {
        if (!visible)
            return;

        auto& preferences = PreferencesConfiguration::Get();
        if (ImGui::Begin("Preferences", &visible, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
        {
            if (ImGui::BeginTable("##preferencesTable", 2, ImGuiTableFlags_SizingStretchSame))
            {
                ImGui::TableNextColumn();
                DrawMiscSection();
                
                ImGui::TableNextColumn();
                DrawFreecamSection();
                DrawOrbitCameraSection();

                ImGui::EndTable();
            }

            ImGui::End();
        }
    }

    void Preferences::Release()
    {
    }
}  // namespace IWXMVM::UI