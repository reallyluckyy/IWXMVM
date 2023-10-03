#include "StdInclude.hpp"
#include "FreeCamera.hpp"

#include "Input.hpp"
#include "UI/UIManager.hpp"

namespace IWXMVM::Components
{
void FreeCamera::Initialize()
{
}

void FreeCamera::Update()
{
    if (!UI::UIManager::Get().GetUIComponent(UI::Component::GameView)->HasFocus())
        return;

    auto& cameraPosition = this->GetPosition();

    // TODO: make this configurable
    constexpr float FREECAM_SPEED = 300;
    constexpr float MOUSE_SPEED = 0.1f;
    constexpr float SCROLL_LOWER_BOUNDARY = -0.001f;
    constexpr float SCROLL_UPPER_BOUNDARY = 0.001f;
    const float SMOOTHING_FACTOR = glm::clamp(1.0f - 15.0f * Input::GetDeltaTime(), 0.0f, 1.0f);

    static double scrollDelta = 0.0;

    auto speedModifier = Input::KeyHeld(ImGuiKey_LeftShift) ? 0.1f : 1.0f;
    speedModifier *= Input::KeyHeld(ImGuiKey_LeftCtrl) ? 3.0f : 1.0f;

    const auto cameraBaseSpeed = Input::GetDeltaTime() * FREECAM_SPEED;
    const auto cameraMovementSpeed = cameraBaseSpeed * speedModifier;
    const auto cameraHeightSpeed = cameraMovementSpeed;

    if (Input::BindHeld(InputConfiguration::BIND_FREE_CAMERA_FORWARD))
    {
        cameraPosition += this->GetForwardVector() * cameraMovementSpeed;
    }

    if (Input::BindHeld(InputConfiguration::BIND_FREE_CAMERA_BACKWARD))
    {
        cameraPosition -= this->GetForwardVector() * cameraMovementSpeed;
    }

    if (Input::BindHeld(InputConfiguration::BIND_FREE_CAMERA_LEFT))
    {
        cameraPosition += this->GetRightVector() * cameraMovementSpeed;
    }

    if (Input::BindHeld(InputConfiguration::BIND_FREE_CAMERA_RIGHT))
    {
        cameraPosition -= this->GetRightVector() * cameraMovementSpeed;
    }

    if (Input::KeyHeld(ImGuiKey_LeftAlt))
    {
        this->GetRotation()[2] += Input::GetScrollDelta();
    }
    else
    {
        scrollDelta -= Input::GetScrollDelta();

        if (scrollDelta < SCROLL_LOWER_BOUNDARY || scrollDelta > SCROLL_UPPER_BOUNDARY)
        {
            this->GetFov() += scrollDelta * Input::GetDeltaTime() * 32.0f;
            this->GetFov() = glm::clamp(this->GetFov(), 1.0f, 179.0f);
            scrollDelta *= SMOOTHING_FACTOR;
        }
        else if (scrollDelta != 0.0)
        {
            scrollDelta = 0.0;
        }
    }

    if (Input::BindHeld(InputConfiguration::BIND_FREE_CAMERA_RESET))
    {
        this->GetRotation() = {};
        this->GetFov() = 90.0f;
    }

    this->GetRotation()[0] += Input::GetMouseDelta()[1] * MOUSE_SPEED;
    this->GetRotation()[1] -= Input::GetMouseDelta()[0] * MOUSE_SPEED;

    this->GetRotation()[0] = glm::clamp(this->GetRotation()[0], -89.9f, 89.9f);

    if (Input::BindHeld(InputConfiguration::BIND_FREE_CAMERA_UP))
        cameraPosition[2] += cameraHeightSpeed;

    if (Input::BindHeld(InputConfiguration::BIND_FREE_CAMERA_DOWN))
        cameraPosition[2] -= cameraHeightSpeed;
}
}  // namespace IWXMVM::Components
