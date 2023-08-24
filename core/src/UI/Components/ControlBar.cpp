#include "StdInclude.hpp"
#include "ControlBar.hpp"

#include "Mod.hpp"
#include "CustomImGuiControls.hpp"
#include "UI/UIImage.hpp"
#include "UI/UIManager.hpp"
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
		// if (Mod::GetGameInterface()->GetGameState() != GameInterface::GameState::InDemo)
			// return;

		if (!timescale.has_value())
		{
			timescale = Mod::GetGameInterface()->GetDvar("timescale");
			return;
		}

		SetPosition(
			0,
			UIManager::uiComponents[UIManager::MENUBAR]->GetSize().y + UIManager::uiComponents[UIManager::GAMEVIEW]->GetSize().y
		);
		SetSize(
			ImGui::GetIO().DisplaySize.x,
			ImGui::GetIO().DisplaySize.y - GetPosition().y
		);

		ImGui::SetNextWindowPos(GetPosition());
		ImGui::SetNextWindowSize(GetSize());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("Playback Controls", nullptr, flags))
		{
			constexpr auto PADDING = 20;
			ImGui::SetCursorPosX(PADDING);
			ImGui::Text("%s", Mod::GetGameInterface()->GetDemoInfo().name.c_str());

			auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
			// TODO: change
			if (*(bool*)0x74e340 == true) {
				ImGui::SameLine(GetSize().x / 4);
				ImGui::DemoProgressBar(&demoInfo.currentTick, demoInfo.endTick, ImVec2(GetSize().x / 2, GetSize().y / 3.4f), std::format("{0}", demoInfo.currentTick).c_str());
			}

			// Right side 

			const auto playbackSpeedSliderWidth = GetSize().x / 8;
			const auto playbackSpeedSliderX = GetSize().x - playbackSpeedSliderWidth - PADDING;

			ImGui::SameLine(playbackSpeedSliderX);
			ImGui::SetNextItemWidth(playbackSpeedSliderWidth);
			ImGui::TimescaleSlider("##", &timescale.value().value->floating_point, 0.001f, 1000.0f, "%.3f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

			auto pauseButtonSize = ImVec2(ImGui::GetFontSize() * 1.4f, ImGui::GetFontSize() * 1.4f);

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

	void ControlBar::Release()
	{
		UIImage::ReleaseResource(IMG_PLAY_BUTTON_data);
		UIImage::ReleaseResource(IMG_PAUSE_BUTTON_data);
	}
}
