#include "StdInclude.hpp"
#include "ControlBar.hpp"

#include "Mod.hpp"
#include "UI/UIImage.hpp"
#include "CustomProgressBar.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM::UI
{
	float playbackSpeed;

	std::optional<Dvar> timescale;

	void ControlBar::Initialize()
	{
		playbackSpeed = 1.0f;
	}

	INCBIN_EXTERN(IMG_PLAY_BUTTON);
	INCBIN_EXTERN(IMG_PAUSE_BUTTON);

	void ControlBar::Render()
	{
		if (Mod::GetGameInterface()->GetGameState() != GameInterface::GameState::InDemo)
			return;

		if (!timescale.has_value())
		{
			timescale = Mod::GetGameInterface()->GetDvar("timescale");
			return;
		}

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
		ImGui::DemoProgressBar(&demoInfo.currentTick, demoInfo.endTick, ImVec2(panelSize.x / 2, panelSize.y / 3.4f), std::format("{0}", demoInfo.currentTick).c_str());

		// Right side 

		const auto playbackSpeedSliderWidth = panelSize.x / 8;
		const auto playbackSpeedSliderX = panelSize.x - playbackSpeedSliderWidth - PADDING;

		ImGui::SameLine(playbackSpeedSliderX);
		ImGui::SetNextItemWidth(playbackSpeedSliderWidth);
		ImGui::SliderFloat("##", (float*)timescale.value().value, 0.001f, 1000.0f, "%.3f", ImGuiSliderFlags_Logarithmic);

		auto pauseButtonSize = ImVec2(panelSize.y / 3.6f, panelSize.y / 3.6f);

		auto image = UIImage::FromResource(Mod::GetGameInterface()->IsDemoPlaybackPaused() ? IMG_PLAY_BUTTON_data : IMG_PAUSE_BUTTON_data,
			Mod::GetGameInterface()->IsDemoPlaybackPaused() ? IMG_PLAY_BUTTON_size : IMG_PAUSE_BUTTON_size);
		ImGui::SameLine(playbackSpeedSliderX - 10 - pauseButtonSize.x);
		if (ImGui::ImageButton(image.GetTextureID(), pauseButtonSize, ImVec2(0, 0), ImVec2(1, 1), 1))
		{
			Mod::GetGameInterface()->ToggleDemoPlaybackState();
		}

		ImGui::End();
	}
}
