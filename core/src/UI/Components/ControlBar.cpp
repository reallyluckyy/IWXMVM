#include "StdInclude.hpp"
#include "ControlBar.hpp"

#include "Mod.hpp"
#include "Components/CameraManager.hpp"
#include "UI/ImGuiExtensions.hpp"
#include "UI/UIImage.hpp"
#include "UI/UIManager.hpp"
#include "Utilities/PathUtils.hpp"
#include "Input.hpp"

namespace IWXMVM::UI
{
    float playbackSpeed;

    std::optional<Types::Dvar> timescale;

    void ControlBar::Initialize()
    {
        playbackSpeed = 1.0f;
    }

    void HandlePlaybackInput()
    {
        // this should probably go somewhere else eventually

        if (Input::BindDown(Action::PlaybackToggle))
        {
            Mod::GetGameInterface()->ToggleDemoPlaybackState();
        }

        if (Input::BindDown(Action::PlaybackFaster))
        {
            timescale.value().value->floating_point *= 2.0f;
        }

        if (Input::BindDown(Action::PlaybackSlower))
        {
            timescale.value().value->floating_point /= 2.0f;
        }

        if (Input::BindDown(Action::PlaybackSkipForward))
        {
            Mod::GetGameInterface()->SetTickDelta(500);
        }

        if (Input::BindDown(Action::PlaybackSkipBackward))
        {
            Mod::GetGameInterface()->SetTickDelta(-500);
        }
    }

    INCBIN_EXTERN(IMG_PLAY_BUTTON);
    INCBIN_EXTERN(IMG_PAUSE_BUTTON);

    void ControlBar::Render()
    {
        if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
            return;

        if (!timescale.has_value())
        {
            timescale = Mod::GetGameInterface()->GetDvar("timescale");
            return;
        }

        HandlePlaybackInput();

        const auto padding = ImGui::GetStyle().WindowPadding;

        SetPosition(0, UIManager::Get().GetUIComponent(UI::Component::MenuBar)->GetSize().y +
                           UIManager::Get().GetUIComponent(UI::Component::GameView)->GetSize().y);
        SetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetFontSize() * 1.4f + padding.y * 2 + 4);

        ImGui::SetNextWindowPos(GetPosition());
        ImGui::SetNextWindowSize(GetSize());

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoTitleBar;
        if (ImGui::Begin("Playback Controls", nullptr, flags))
        {
            ImGui::SetCursorPosX(padding.x);

            auto pauseButtonSize = ImVec2(ImGui::GetFontSize() * 1.4f, ImGui::GetFontSize() * 1.4f);

            auto image = UIImage::FromResource(
                Mod::GetGameInterface()->IsDemoPlaybackPaused() ? IMG_PLAY_BUTTON_data : IMG_PAUSE_BUTTON_data,
                Mod::GetGameInterface()->IsDemoPlaybackPaused() ? IMG_PLAY_BUTTON_size : IMG_PAUSE_BUTTON_size);

            if (ImGui::ImageButton(image.GetTextureID(), pauseButtonSize, ImVec2(0, 0), ImVec2(1, 1), 1))
                Mod::GetGameInterface()->ToggleDemoPlaybackState();

            const auto playbackSpeedSliderWidth = GetSize().x / 8;

            ImGui::SetNextItemWidth(playbackSpeedSliderWidth);
            ImGui::SameLine();
            ImGuiEx::TimescaleSlider("##1", &timescale.value().value->floating_point, 0.001f, 1000.0f, "%.3f",
                                     ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

            auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
            auto progressBarX = padding.x + pauseButtonSize.x + playbackSpeedSliderWidth + padding.x * 3;
            auto progressBarWidth = GetSize().x - progressBarX - GetSize().x * 0.05f - padding.x;

            ImGui::SameLine(progressBarX);
            ImGui::SetNextItemWidth(progressBarWidth);
            static std::int32_t tickValue{};

            if (!ImGuiEx::DemoProgressBar(&tickValue, 0, demoInfo.endTick))
                tickValue = demoInfo.currentTick;
            else
                Mod::GetGameInterface()->SetTickDelta(tickValue - demoInfo.currentTick);

            ImGui::SameLine();
            ImGui::Text("%s", std::format("{0}", demoInfo.currentTick).c_str());

            ImGui::End();
        }

        UIManager::Get().GetUIComponent(UI::Component::KeyframeEditor)->Render();
    }

    void ControlBar::Release()
    {
        UIImage::ReleaseResource(IMG_PLAY_BUTTON_data);
        UIImage::ReleaseResource(IMG_PAUSE_BUTTON_data);
    }
}  // namespace IWXMVM::UI
