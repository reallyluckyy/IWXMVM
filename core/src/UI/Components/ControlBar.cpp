#include "StdInclude.hpp"
#include "ControlBar.hpp"

#include "Mod.hpp"
#include "UI/UIImage.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM::UI
{
	float playbackSpeed;
	bool isPlaying;

	void ControlBar::Initialize()
	{
		playbackSpeed = 1.0f;
		isPlaying = true;
	}

	INCBIN_EXTERN(IMG_PLAY_BUTTON);
	INCBIN_EXTERN(IMG_PAUSE_BUTTON);

	void ControlBar::Render()
	{
		if (Mod::GetGameInterface()->GetGameState() != GameInterface::GameState::InDemo)
			return;

		const auto PADDING = 20;

		auto windowSize = PathUtils::GetWindowSize(Mod::GetGameInterface()->GetWindowHandle());

		auto panelSize = ImVec2(windowSize.x, windowSize.y / 12);

		ImGui::Begin("Playback Controls", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowSize(panelSize);
		ImGui::SetWindowPos(ImVec2(windowSize.x - panelSize.x, windowSize.y - panelSize.y));

		ImGui::SetCursorPosX(PADDING);
		ImGui::Text("%s", Mod::GetGameInterface()->GetDemoInfo().name.c_str());

		auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
		ImGui::SameLine(panelSize.x / 4);
		ImGui::ProgressBar((float)demoInfo.currentTick / (float)demoInfo.endTick, ImVec2(panelSize.x / 2, panelSize.y / 3.4f), std::format("{0}", demoInfo.currentTick).c_str());

		// Right side 

		const auto playbackSpeedSliderWidth = panelSize.x / 8;
		const auto playbackSpeedSliderX = panelSize.x - playbackSpeedSliderWidth - PADDING;

		ImGui::SameLine(playbackSpeedSliderX);
		ImGui::SetNextItemWidth(playbackSpeedSliderWidth);
		ImGui::SliderFloat("##", &playbackSpeed, 0.01f, 50.0f, "%.3f", ImGuiSliderFlags_Logarithmic);

		auto pauseButtonSize = ImVec2(panelSize.y / 3.6f, panelSize.y / 3.6f);

		auto image = UIImage::FromResource(isPlaying ? IMG_PAUSE_BUTTON_data : IMG_PLAY_BUTTON_data, isPlaying ? IMG_PAUSE_BUTTON_size : IMG_PLAY_BUTTON_size);
		ImGui::SameLine(playbackSpeedSliderX - 10 - pauseButtonSize.x);
		if (ImGui::ImageButton(image.GetTextureID(), pauseButtonSize, ImVec2(0, 0), ImVec2(1, 1), 1))
		{
			isPlaying = !isPlaying;
		}

		ImGui::End();
	}
}
