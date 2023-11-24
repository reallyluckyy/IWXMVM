#include "StdInclude.hpp"
#include "OrbitCamera.hpp"

#include "Input.hpp"
#include "Utilities/MathUtils.hpp"
#include "UI/UIManager.hpp"
#include "UI/ImGuiEx/ImGuiExtensions.hpp"
#include "Events.hpp"
#include "Components/CameraManager.hpp"

namespace IWXMVM::Components
{
    void OrbitCamera::Initialize()
    {
        Events::RegisterListener(EventType::OnRenderGameView, [&]() {
            if (UI::UIManager::Get().GetUIComponent(UI::Component::GameView)->HasFocus() &&
                CameraManager::Get().GetActiveCamera()->GetMode() == Camera::Mode::Orbit)
            {
                DrawOverlay();
            }
        });
    }

    void OrbitCamera::Update()
    {
        if (!UI::UIManager::Get().GetUIComponent(UI::Component::GameView)->HasFocus())
            return;

        auto& cameraPosition = this->GetPosition();

        constexpr float BASE_SPEED = 0.1f;
        constexpr float ROTATION_SPEED = BASE_SPEED * 1.5f;
        constexpr float TRANSLATION_SPEED = BASE_SPEED * 3.0f;
        constexpr float ZOOM_SPEED = BASE_SPEED * 8.0f;
        constexpr float HEIGHT_CEILING = 250.0f;
        constexpr float HEIGHT_MULTIPLIER = 1.5f;
        constexpr float SCROLL_LOWER_BOUNDARY = -0.001f;
        constexpr float SCROLL_UPPER_BOUNDARY = 0.001f;
        constexpr float MIN_ORBIT_DIST = 10;
        const float SMOOTHING_FACTOR = glm::clamp(1.0f - 10.0f * Input::GetDeltaTime(), 0.0f, 1.0f);

        static double scrollDelta = 0.0;
        scrollDelta -= Input::GetScrollDelta() * ZOOM_SPEED;

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
            auto horizontalDelta = -Input::GetMouseDelta()[0] * ROTATION_SPEED;
            cameraPosition -= orbitCameraOrigin;
            cameraPosition = glm::rotateZ(cameraPosition, glm::radians(horizontalDelta));
            cameraPosition += orbitCameraOrigin;

            auto verticalDelta = Input::GetMouseDelta()[1] * ROTATION_SPEED;
            cameraPosition -= orbitCameraOrigin;
            cameraPosition = glm::rotate(cameraPosition, glm::radians(verticalDelta),
                                         glm::cross(glm::vector3::up, this->GetForwardVector()));
            cameraPosition += orbitCameraOrigin;
        }

        if (Input::BindHeld(Action::OrbitCameraRotate) && Input::KeyHeld(ImGuiKey_LeftShift))
        {
            // use the height value to move faster around at higher altitude
            const float translationSpeed = TRANSLATION_SPEED + HEIGHT_MULTIPLIER *
                                                                   (std::abs(cameraPosition[2]) / HEIGHT_CEILING) *
                                                                   TRANSLATION_SPEED;

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

    void OrbitCamera::DrawOverlay()
    {
        if (Input::BindHeld(Action::OrbitCameraRotate))
        {
            DrawGrid();
        }

        // DrawOrbitPoint();
    }

    void OrbitCamera::DrawGrid()
    {
        auto& gameView = UI::UIManager::Get().GetUIComponent(UI::Component::GameView);
        auto viewport = glm::vec4(gameView->GetPosition().x, gameView->GetPosition().y,
                                  gameView->GetPosition().x + gameView->GetSize().x,
                                  gameView->GetPosition().y + gameView->GetSize().y);
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(viewport.x, viewport.y), ImVec2(viewport.z, viewport.w),
                                                  ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 0.6f)));

        const auto GLOBAL_AXIS_LENGTH = 3000.0f;
        const auto MAIN_COLOR = ImVec4(0.4f, 0.4f, 0.4f, 0.6f);
        const auto SECONDARY_COLOR = ImVec4(0.5f, 0.5f, 0.5f, 0.4f);

        ImGuiEx::DrawLine3D(glm::vec3(-GLOBAL_AXIS_LENGTH, 0, 0), glm::vec3(GLOBAL_AXIS_LENGTH, 0, 0), MAIN_COLOR, 5);
        ImGuiEx::DrawLine3D(glm::vec3(0, -GLOBAL_AXIS_LENGTH, 0), glm::vec3(0, GLOBAL_AXIS_LENGTH, 0), MAIN_COLOR, 5);

        for (int i = -GLOBAL_AXIS_LENGTH; i < GLOBAL_AXIS_LENGTH; i += 100)
        {
            ImGuiEx::DrawLine3D(glm::vec3(-GLOBAL_AXIS_LENGTH, i, 0), glm::vec3(GLOBAL_AXIS_LENGTH, i, 0),
                                SECONDARY_COLOR, 3);
            ImGuiEx::DrawLine3D(glm::vec3(i, -GLOBAL_AXIS_LENGTH, 0), glm::vec3(i, GLOBAL_AXIS_LENGTH, 0),
                                SECONDARY_COLOR, 3);
        }
    }

    void OrbitCamera::DrawOrbitPoint()
    {
        const auto AXIS_LENGTH = 150.0f;

        ImGuiEx::DrawLine3D(orbitCameraOrigin - glm::vec3(AXIS_LENGTH, 0, 0),
                            orbitCameraOrigin + glm::vec3(AXIS_LENGTH, 0, 0), ImVec4(1.0f, 0.0f, 0.0f, 0.8f), 5);
        ImGuiEx::DrawLine3D(orbitCameraOrigin - glm::vec3(0, AXIS_LENGTH, 0),
                            orbitCameraOrigin + glm::vec3(0, AXIS_LENGTH, 0), ImVec4(0.0f, 1.0f, 0.0f, 0.8f), 5);
        ImGuiEx::DrawLine3D(orbitCameraOrigin - glm::vec3(0, 0, AXIS_LENGTH),
                            orbitCameraOrigin + glm::vec3(0, 0, AXIS_LENGTH), ImVec4(0.0f, 0.0f, 1.0f, 0.8f), 5);
    }
}  // namespace IWXMVM::Components
