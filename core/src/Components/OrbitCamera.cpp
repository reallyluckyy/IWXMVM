#include "StdInclude.hpp"
#include "OrbitCamera.hpp"

#include "Input.hpp"
#include "Utilities/MathUtils.hpp"
#include "UI/UIManager.hpp"

namespace IWXMVM::Components
{
	void OrbitCamera::UpdateMovement(Camera& camera)
	{
		auto& cameraPosition = camera.GetPosition();

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
			cameraPosition = orbitCameraOrigin + glm::vector3::one;
		}

		if (Input::BindHeld("orbitCameraReset"))
		{
			scrollDelta = 0.0;

			orbitCameraOrigin = glm::vector3::zero;
			cameraPosition = glm::vector3::one;
		}

		if (Input::BindHeld("orbitCameraRotate"))
		{
			auto horizontalDelta = -Input::GetMouseDelta()[0] * ROTATION_SPEED;
			cameraPosition -= orbitCameraOrigin;
			cameraPosition = glm::rotateZ(cameraPosition, glm::radians(horizontalDelta));
			cameraPosition += orbitCameraOrigin;

			auto verticalDelta = Input::GetMouseDelta()[1] * ROTATION_SPEED;
			cameraPosition -= orbitCameraOrigin;
			cameraPosition = glm::rotate(cameraPosition, glm::radians(verticalDelta), glm::cross(glm::vector3::up, camera.GetForwardVector()));
			cameraPosition += orbitCameraOrigin;
		}

		if (Input::BindHeld("orbitCameraMove"))
		{
			DrawGrid(camera);

			// use the height value to move faster around at higher altitude 
			const float translationSpeed = TRANSLATION_SPEED + HEIGHT_MULTIPLIER * (std::abs(cameraPosition[2]) / HEIGHT_CEILING) * TRANSLATION_SPEED;

			glm::vec3 forward2D = glm::normalize(camera.GetForwardVector());
			forward2D.z = 0;
			orbitCameraOrigin += forward2D * Input::GetMouseDelta()[1] * translationSpeed;
			cameraPosition += forward2D * Input::GetMouseDelta()[1] * translationSpeed;

			glm::vec3 right2D = glm::normalize(camera.GetRightVector());
			right2D.z = 0;
			orbitCameraOrigin += right2D * Input::GetMouseDelta()[0] * translationSpeed;
			cameraPosition += right2D * Input::GetMouseDelta()[0] * translationSpeed;
		}

		camera.SetForwardVector(orbitCameraOrigin - cameraPosition);

		if (scrollDelta < SCROLL_LOWER_BOUNDARY || scrollDelta > SCROLL_UPPER_BOUNDARY)
		{
			auto desiredPosition = cameraPosition + glm::normalize(cameraPosition - orbitCameraOrigin) * ((1.0f - SMOOTHING_FACTOR) * scrollDelta) * 100;
			if (glm::distance(desiredPosition, orbitCameraOrigin) > MIN_ORBIT_DIST)
			{
				cameraPosition = desiredPosition;
			}
			else
			{
				cameraPosition = orbitCameraOrigin + glm::normalize(cameraPosition - orbitCameraOrigin) * MIN_ORBIT_DIST;
			}
			scrollDelta *= SMOOTHING_FACTOR;
		}
		else if (scrollDelta != 0.0)
		{
			scrollDelta = 0.0;
		}

		DrawOrbitPoint(camera);
	}

	void DrawLine(Camera& camera, glm::vec3 from, glm::vec3 to, ImVec4 color, float thickness)
	{
		auto& gameView = UI::UIManager::Get().GetUIComponent(UI::Component::GameView);
		auto viewport = glm::vec4(gameView->GetPosition().x, gameView->GetPosition().y, gameView->GetPosition().x + gameView->GetSize().x, gameView->GetPosition().y + gameView->GetSize().y);
		auto screenPosition1 = MathUtils::WorldToScreenPoint(from, camera, viewport);
		auto screenPosition2 = MathUtils::WorldToScreenPoint(to, camera, viewport);

		if (screenPosition1.has_value() && screenPosition2.has_value())
		{
			ImGui::GetForegroundDrawList()->AddLine(screenPosition1.value(), screenPosition2.value(), ImGui::ColorConvertFloat4ToU32(color), thickness);
		}
	}

	void DrawPoint(Camera& camera, glm::vec3 point, ImVec4 color) 
	{
		auto& gameView = UI::UIManager::Get().GetUIComponent(UI::Component::GameView);
		auto viewport = glm::vec4(gameView->GetPosition().x, gameView->GetPosition().y, gameView->GetPosition().x + gameView->GetSize().x, gameView->GetPosition().y + gameView->GetSize().y);
		auto screenPosition = MathUtils::WorldToScreenPoint(point, camera, viewport);

		if (screenPosition.has_value())
		{
			ImGui::GetForegroundDrawList()->AddRectFilled(screenPosition.value() - ImVec2(3, 3), screenPosition.value() + ImVec2(3, 3), ImGui::ColorConvertFloat4ToU32(color));
		}
	}

	void OrbitCamera::DrawGrid(Camera& camera)
	{
		auto& gameView = UI::UIManager::Get().GetUIComponent(UI::Component::GameView);
		auto viewport = glm::vec4(gameView->GetPosition().x, gameView->GetPosition().y, gameView->GetPosition().x + gameView->GetSize().x, gameView->GetPosition().y + gameView->GetSize().y);
		ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(viewport.x, viewport.y), ImVec2(viewport.z, viewport.w), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 0.6f)));

		const auto GLOBAL_AXIS_LENGTH = 3000.0f;

		DrawLine(camera, glm::vec3(-GLOBAL_AXIS_LENGTH, 0, 0), glm::vec3(GLOBAL_AXIS_LENGTH, 0, 0), ImVec4(0.4f, 0.4f, 0.4f, 0.6f), 5);
		DrawLine(camera, glm::vec3(0, -GLOBAL_AXIS_LENGTH, 0), glm::vec3(0, GLOBAL_AXIS_LENGTH, 0), ImVec4(0.4f, 0.4f, 0.4f, 0.6f), 5);

		for (int i = -GLOBAL_AXIS_LENGTH; i < GLOBAL_AXIS_LENGTH; i += 100)
		{
			DrawLine(camera, glm::vec3(-GLOBAL_AXIS_LENGTH, i, 0), glm::vec3(GLOBAL_AXIS_LENGTH, i, 0), ImVec4(0.5f, 0.5f, 0.5f, 0.4f), 3);
			DrawLine(camera, glm::vec3(i, -GLOBAL_AXIS_LENGTH, 0), glm::vec3(i, GLOBAL_AXIS_LENGTH, 0), ImVec4(0.5f, 0.5f, 0.5f, 0.4f), 3);
		}
	}

	void OrbitCamera::DrawOrbitPoint(Camera& camera)
	{
		const auto AXIS_LENGTH = 150.0f;

		DrawLine(camera, orbitCameraOrigin - glm::vec3(AXIS_LENGTH, 0, 0), orbitCameraOrigin + glm::vec3(AXIS_LENGTH, 0, 0), ImVec4(1.0f, 0.0f, 0.0f, 0.8f), 5);
		DrawLine(camera, orbitCameraOrigin - glm::vec3(0, AXIS_LENGTH, 0), orbitCameraOrigin + glm::vec3(0, AXIS_LENGTH, 0), ImVec4(0.0f, 1.0f, 0.0f, 0.8f), 5);
		DrawLine(camera, orbitCameraOrigin - glm::vec3(0, 0, AXIS_LENGTH), orbitCameraOrigin + glm::vec3(0, 0, AXIS_LENGTH), ImVec4(0.0f, 0.0f, 1.0f, 0.8f), 5);
	}
}
