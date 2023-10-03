#include "StdInclude.hpp"
#include "ControlBar.hpp"

#include "Mod.hpp"
#include "Components/CameraManager.hpp"
#include "CustomImGuiControls.hpp"
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

    if (Input::BindDown(InputConfiguration::BIND_PLAYBACK_TOGGLE))
    {
        Mod::GetGameInterface()->ToggleDemoPlaybackState();
    }

    if (Input::BindDown(InputConfiguration::BIND_PLAYBACK_FASTER))
    {
        timescale.value().value->floating_point *= 2.0f;
    }

    if (Input::BindDown(InputConfiguration::BIND_PLAYBACK_SLOWER))
    {
        timescale.value().value->floating_point /= 2.0f;
    }

    if (Input::BindDown(InputConfiguration::BIND_PLAYBACK_SKIP_FORWARD))
    {
        Mod::GetGameInterface()->SetTickDelta(500);
    }

    if (Input::BindDown(InputConfiguration::BIND_PLAYBACK_SKIP_FORWARD))
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

    SetPosition(0, UIManager::Get().GetUIComponent(UI::Component::MenuBar)->GetSize().y +
                       UIManager::Get().GetUIComponent(UI::Component::GameView)->GetSize().y);
    SetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - GetPosition().y);

    ImGui::SetNextWindowPos(GetPosition());
    ImGui::SetNextWindowSize(GetSize());

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
    if (ImGui::Begin("Playback Controls", nullptr, flags))
    {
        constexpr auto PADDING = 20;
        ImGui::SetCursorPosX(PADDING);

        auto pauseButtonSize = ImVec2(ImGui::GetFontSize() * 1.4f, ImGui::GetFontSize() * 1.4f);

        auto image = UIImage::FromResource(
            Mod::GetGameInterface()->IsDemoPlaybackPaused() ? IMG_PLAY_BUTTON_data : IMG_PAUSE_BUTTON_data,
            Mod::GetGameInterface()->IsDemoPlaybackPaused() ? IMG_PLAY_BUTTON_size : IMG_PAUSE_BUTTON_size);

        if (ImGui::ImageButton(image.GetTextureID(), pauseButtonSize, ImVec2(0, 0), ImVec2(1, 1), 1))
            Mod::GetGameInterface()->ToggleDemoPlaybackState();

        const auto playbackSpeedSliderWidth = GetSize().x / 8;

        ImGui::SetNextItemWidth(playbackSpeedSliderWidth);
        ImGui::SameLine();
        ImGui::TimescaleSlider("##1", &timescale.value().value->floating_point, 0.001f, 1000.0f, "%.3f",
                               ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

        auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
        auto progressBarX = PADDING + pauseButtonSize.x + playbackSpeedSliderWidth + PADDING * 3;
        auto progressBarWidth = GetSize().x - progressBarX - GetSize().x * 0.05f - PADDING;

        ImGui::SameLine(progressBarX);
        static std::int32_t tickValue{};

        if (!ImGui::SliderInt("##2", &tickValue, 0, demoInfo.endTick, "%d", ImGuiSliderFlags_NoInput))
            tickValue = demoInfo.currentTick;
        else
            Mod::GetGameInterface()->SetTickDelta(tickValue - demoInfo.currentTick);

        ImGui::SameLine(progressBarX);
        ImGui::DemoProgressBarLines(demoInfo.currentTick, demoInfo.endTick);

        ImGui::SameLine(progressBarX);
        ImGui::TimelineMarkers(Components::CampathManager::Get().GetMarkers(), demoInfo.endTick);

        ImGui::SameLine();
        ImGui::Text("%s", std::format("{0}", demoInfo.currentTick).c_str());

        ImGui::End();
    }
}

void ControlBar::Release()
{
    UIImage::ReleaseResource(IMG_PLAY_BUTTON_data);
    UIImage::ReleaseResource(IMG_PAUSE_BUTTON_data);
}
}  // namespace IWXMVM::UI
