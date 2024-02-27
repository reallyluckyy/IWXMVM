#include "StdInclude.hpp"
#include "OrbitCamera.hpp"

#include "Input.hpp"
#include "Utilities/MathUtils.hpp"
#include "UI/UIManager.hpp"
#include "UI/ImGuiEx/ImGuiExtensions.hpp"
#include "Events.hpp"
#include "Components/CameraManager.hpp"
#include "Configuration/PreferencesConfiguration.hpp"

namespace IWXMVM::Components
{
    void OrbitCamera::Initialize()
    {
    }

    void OrbitCamera::Update()
    {
        if (!UI::UIManager::Get().GetUIComponent(UI::Component::GameView)->HasFocus())
            return;

        auto& cameraPosition = this->GetPosition();
        auto& preferences = PreferencesConfiguration::Get();

        constexpr float HEIGHT_CEILING = 250.0f;
        constexpr float HEIGHT_MULTIPLIER = 1.5f;
        constexpr float SCROLL_LOWER_BOUNDARY = -0.001f;
        constexpr float SCROLL_UPPER_BOUNDARY = 0.001f;
        constexpr float MIN_ORBIT_DIST = 10;
        const float SMOOTHING_FACTOR = glm::clamp(1.0f - 10.0f * Input::GetDeltaTime(), 0.0f, 1.0f);

        static double scrollDelta = 0.0;
        scrollDelta -= Input::GetScrollDelta() * preferences.orbitZoomSpeed;

        // bump camera out of origin if it's at the origin
        if (cameraPosition == orbitCameraOrigin)
        {
            cameraPosition = orbitCameraOrigin + glm::vector3::one * 100;
        }

        if (Input::BindHeld(Action::OrbitCameraReset))
        {
            scrollDelta = 0.0;

            orbitCameraOrigin = glm::vector3::zero;
            cameraPosition = glm::vector3::one * 100;
        }

        if (Input::BindHeld(Action::OrbitCameraRotate) && !Input::KeyHeld(ImGuiKey_LeftShift))
        {
            auto horizontalDelta = -Input::GetMouseDelta()[0] * preferences.orbitRotationSpeed;
            cameraPosition -= orbitCameraOrigin;
            cameraPosition = glm::rotateZ(cameraPosition, glm::radians(horizontalDelta));
            cameraPosition += orbitCameraOrigin;

            auto verticalDelta = Input::GetMouseDelta()[1] * preferences.orbitRotationSpeed;
            cameraPosition -= orbitCameraOrigin;
            cameraPosition = glm::rotate(cameraPosition, glm::radians(verticalDelta),
                                         glm::cross(glm::vector3::up, this->GetForwardVector()));
            cameraPosition += orbitCameraOrigin;
        }

        if (Input::BindHeld(Action::OrbitCameraRotate) && Input::KeyHeld(ImGuiKey_LeftShift))
        {
            // use the height value to move faster around at higher altitude
            const float translationSpeed =
                preferences.orbitMoveSpeed + HEIGHT_MULTIPLIER * (std::abs(cameraPosition[2]) / HEIGHT_CEILING) * preferences.orbitMoveSpeed;

            glm::vec3 forward2D = glm::normalize(this->GetForwardVector());
            forward2D.z = 0;
            orbitCameraOrigin += forward2D * Input::GetMouseDelta()[1] * translationSpeed;
            cameraPosition += forward2D * Input::GetMouseDelta()[1] * translationSpeed;

            glm::vec3 right2D = glm::normalize(this->GetRightVector());
            right2D.z = 0;
            orbitCameraOrigin += right2D * Input::GetMouseDelta()[0] * translationSpeed;
            cameraPosition += right2D * Input::GetMouseDelta()[0] * translationSpeed;
        }

        this->SetForwardVector(orbitCameraOrigin - cameraPosition);

        if (scrollDelta < SCROLL_LOWER_BOUNDARY || scrollDelta > SCROLL_UPPER_BOUNDARY)
        {
            auto desiredPosition = cameraPosition + glm::normalize(cameraPosition - orbitCameraOrigin) *
                                                        ((1.0f - SMOOTHING_FACTOR) * scrollDelta) * 100;
            if (glm::distance(desiredPosition, orbitCameraOrigin) > MIN_ORBIT_DIST)
            {
                cameraPosition = desiredPosition;
            }
            else
            {
                cameraPosition =
                    orbitCameraOrigin + glm::normalize(cameraPosition - orbitCameraOrigin) * MIN_ORBIT_DIST;
            }
            scrollDelta *= SMOOTHING_FACTOR;
        }
        else if (scrollDelta != 0.0)
        {
            scrollDelta = 0.0;
        }
    }
}  // namespace IWXMVM::Components
