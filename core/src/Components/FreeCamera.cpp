#include "StdInclude.hpp"
#include "FreeCamera.hpp"

#include "Input.hpp"
#include "Events.hpp"
#include "Resources.hpp"
#include "UI/UIManager.hpp"
#include "Components/CameraManager.hpp"
#include "Configuration/PreferencesConfiguration.hpp"

namespace IWXMVM::Components
{
    constexpr float PROPERTY_DISPLAY_TIME = 4.0f;
    float fovDisplayTimer = 0.0f;
    float rotationDisplayTimer = 0.0f;

    int GetAlphaForTimer(float timer)
    {
        return (int)(timer / PROPERTY_DISPLAY_TIME * 2 * 255.0f);
    }

    void FreeCamera::Initialize()
    {
        Events::RegisterListener(EventType::OnRenderGameView, [&]() {

            if (CameraManager::Get().GetActiveCamera()->GetMode() != Camera::Mode::Free)
                return;

            auto& gameView = UI::UIManager::Get().GetUIComponent(UI::Component::GameView);

            if (fovDisplayTimer > 0)
            {
                auto label = std::format(ICON_FA_VIDEO " {0:.0f} ", this->GetFov());
                auto pos = gameView->GetPosition() + gameView->GetSize() -
                           ImVec2(ImGui::GetFontSize() * 5, ImGui::GetFontSize() * 2) -
                           ImGui::CalcTextSize(label.c_str());
                ImGui::GetWindowDrawList()->AddText(pos,
                                                    ImColor(255, 255, 255, GetAlphaForTimer(fovDisplayTimer)),
                                                    label.c_str());
                fovDisplayTimer -= Input::GetDeltaTime();
            }

            if (rotationDisplayTimer > 0)
            {
                auto label = std::format(ICON_FA_ARROW_ROTATE_RIGHT " {0:.0f} ", this->GetRotation()[2]);
                auto pos = gameView->GetPosition() +
                           ImVec2(ImGui::GetFontSize() * 5, gameView->GetSize().y - ImGui::GetFontSize() * 2) - 
                           ImVec2(0, ImGui::CalcTextSize(label.c_str()).y);
                ImGui::GetWindowDrawList()->AddText(pos, ImColor(255, 255, 255, GetAlphaForTimer(rotationDisplayTimer)),
                                                    label.c_str());
                rotationDisplayTimer -= Input::GetDeltaTime();
            }
        });
    }

    void FreeCamera::Update()
    {
        if (!UI::UIManager::Get().GetUIComponent(UI::Component::GameView)->HasFocus())
            return;

        HandleFreecamInput(this->position, this->rotation, this->fov, this->GetForwardVector(), this->GetRightVector());
    }

    void FreeCamera::HandleFreecamInput(glm::vec3& position, glm::vec3& rotation, float& fov, glm::vec3 forward, glm::vec3 right)
    {
        auto& preferences = PreferencesConfiguration::Get();

        constexpr float SCROLL_LOWER_BOUNDARY = -0.001f;
        constexpr float SCROLL_UPPER_BOUNDARY = 0.001f;
        const float SMOOTHING_FACTOR = glm::clamp(1.0f - 15.0f * Input::GetDeltaTime(), 0.0f, 1.0f);

        static double scrollDelta = 0.0;

        auto speedModifier = Input::KeyHeld(ImGuiKey_LeftCtrl) ? 0.1f : 1.0f;
        speedModifier *= Input::KeyHeld(ImGuiKey_LeftShift) ? 3.0f : 1.0f;

        const auto cameraBaseSpeed = Input::GetDeltaTime() * preferences.freecamSpeed;
        const auto cameraMovementSpeed = cameraBaseSpeed * speedModifier;
        const auto cameraHeightSpeed = cameraMovementSpeed;

        if (Input::BindHeld(Action::FreeCameraForward))
        {
            position += forward * cameraMovementSpeed;
        }

        if (Input::BindHeld(Action::FreeCameraBackward))
        {
            position -= forward * cameraMovementSpeed;
        }

        if (Input::BindHeld(Action::FreeCameraLeft))
        {
            position += right * cameraMovementSpeed;
        }

        if (Input::BindHeld(Action::FreeCameraRight))
        {
            position -= right * cameraMovementSpeed;
        }

        if (Input::KeyHeld(ImGuiKey_LeftAlt))
        {
            rotation[2] += Input::GetScrollDelta();
            rotationDisplayTimer = PROPERTY_DISPLAY_TIME;
        }
        else
        {
            scrollDelta -= Input::GetScrollDelta();

            if (scrollDelta < SCROLL_LOWER_BOUNDARY || scrollDelta > SCROLL_UPPER_BOUNDARY)
            {
                fov += scrollDelta * Input::GetDeltaTime() * 32.0f;
                fov = glm::clamp(fov, 1.0f, 179.0f);
                scrollDelta *= SMOOTHING_FACTOR;
                fovDisplayTimer = PROPERTY_DISPLAY_TIME;
            }
            else if (scrollDelta != 0.0)
            {
                scrollDelta = 0.0;
            }
        }

        if (Input::BindHeld(Action::FreeCameraReset))
        {
            rotation = {};
            fov = 90.0f;
        }

        rotation[0] += Input::GetMouseDelta()[1] * preferences.freecamMouseSpeed * fov / 90;
        rotation[1] -= Input::GetMouseDelta()[0] * preferences.freecamMouseSpeed * fov / 90;
        
        rotation[0] = glm::clamp(rotation[0], -89.9f, 89.9f);

        if (Input::BindHeld(Action::FreeCameraUp))
            position[2] += cameraHeightSpeed;

        if (Input::BindHeld(Action::FreeCameraDown))
            position[2] -= cameraHeightSpeed;
    }
}  // namespace IWXMVM::Components
