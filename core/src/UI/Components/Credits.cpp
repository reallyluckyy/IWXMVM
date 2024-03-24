#include "StdInclude.hpp"
#include "Credits.hpp"

#include "Configuration/PreferencesConfiguration.hpp"
#include "UI/UIManager.hpp"

namespace IWXMVM::UI
{
    void Credits::Initialize()
    {
    }

    void Credits::Render()
    {
        if (!visible)
            return;

        ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowSize(ImVec2(300, 400));
        if (ImGui::Begin("Credits", &visible, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::PushFont(UIManager::Get().GetBoldFont());
            ImGui::Text("Contributors");
            ImGui::PopFont();

            ImGui::Text("luckyy");
            ImGui::Text("Anomaly");
            ImGui::Text("Res");
            ImGui::Text("kruumy");
            ImGui::Text("mollyaitch");

            ImGui::Dummy(ImVec2(0, 20));

            ImGui::PushFont(UIManager::Get().GetBoldFont());
            ImGui::Text("Special Thanks to");
            ImGui::PopFont();

            ImGui::Text("xoxor4d");
            ImGui::Text("Gmzorz");

            ImGui::Dummy(ImVec2(50, 50));

            ImGui::End();
        }
    }

    void Credits::Release()
    {
    }
}  // namespace IWXMVM::UI