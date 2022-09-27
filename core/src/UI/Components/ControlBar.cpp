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
		ImGui::SliderFloat("##", &timescale.value().value->floating_point, 0.001f, 1000.0f, "%.3f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

		static constexpr std::array timescales{ 0.001, 0.002, 0.005, 0.0075, 0.01, 0.02, 0.025, 0.05, 0.075, 0.1, 0.125, 0.2, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0, 500.0, 1000.0 };
		for (std::size_t i = 1; i < timescales.size(); ++i) 
		{
			if (timescale.value().value->floating_point > timescales[i - 1] + 0.0000001 && timescale.value().value->floating_point < timescales[i] - 0.0000001)
			{
				timescale.value().value->floating_point = timescales[i - 1];
				break;
			}
		}

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
