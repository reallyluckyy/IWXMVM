#include "StdInclude.hpp"
#include "CaptureMenu.hpp"

#include "UI/UIManager.hpp"

namespace IWXMVM::UI
{
	void CaptureMenu::Initialize()
	{

	}

	void CaptureMenu::Render()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("Capture", NULL, flags))
		{
			ImGui::Text("Camera:");
			ImGui::SameLine();
			const char* cameraComboItems[] = { "First Person Camera", "Dolly Camera", "Bone Camera" };
			static int currentCameraComboItem = 0;
			ImGui::SetNextItemWidth(200);
			ImGui::Combo("##captureMenuCameraCombo", &currentCameraComboItem, cameraComboItems, IM_ARRAYSIZE(cameraComboItems));

			ImGui::Text("Timeframe:");
			ImGui::SameLine();
			static int startTick = 12000;
			ImGui::SetNextItemWidth(100);
			ImGui::InputInt("##startTickInput", &startTick);
			ImGui::SameLine();
			ImGui::Text(" to ");
			ImGui::SameLine();
			static int endTick = 25000;
			ImGui::SetNextItemWidth(100);
			ImGui::InputInt("##endTickInput", &endTick);

			ImGui::Text("Output Format:");
			ImGui::SameLine();
			const char* outputFormatComboItems[] = { "Video (.avi)", "Camera Data (.csv)" };
			static int currentOutputFormatComboItem = 0;
			ImGui::SetNextItemWidth(200);
			ImGui::Combo("##captureMenuOutputFormatCombo", &currentOutputFormatComboItem, outputFormatComboItems, IM_ARRAYSIZE(outputFormatComboItems));

			ImGui::Text("Resolution:");
			ImGui::SameLine();
			const char* resolutionComboItems[] = { "2560x1440", "1920x1080", "1280x720" };
			static int currentResolutionComboItem = 0;
			ImGui::SetNextItemWidth(200);
			ImGui::Combo("##captureMenuResolutionCombo", &currentResolutionComboItem, resolutionComboItems, IM_ARRAYSIZE(resolutionComboItems));

			ImGui::Text("Framerate:");
			ImGui::SameLine();
			const char* framerateComboItems[] = { "250", "500", "1000" };
			static int currentFramerateComboItem = 0;
			ImGui::SetNextItemWidth(200);
			ImGui::Combo("##captureMenuResolutionCombo", &currentFramerateComboItem, framerateComboItems, IM_ARRAYSIZE(framerateComboItems));

			ImGui::End();
		}
	}

	void CaptureMenu::Release()
	{
	}
}