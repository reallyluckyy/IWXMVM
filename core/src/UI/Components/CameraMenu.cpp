#include "StdInclude.hpp"
#include "CameraMenu.hpp"

#include "D3D9.hpp"
#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "UI/ImGuiEx/ImGuiExtensions.hpp"
#include "UI/ImGuiEx/KeyframeableControls.hpp"
#include "Components/CameraManager.hpp"
#include "Components/ShakeManager.hpp"
#include "Input.hpp"
#include "Events.hpp"
#include "Utilities/MathUtils.hpp"
#include "Resources.hpp"

namespace IWXMVM::UI
{
    float fov = 90;

    void CameraMenu::Initialize()
    {
    }

    void DrawFirstPersonSettings()
    {
        auto columnPercent = 0.4f;

        auto& campathManager = Components::CampathManager::Get();
        auto& cameraManager = Components::CameraManager::Get();
        auto& camera = cameraManager.GetActiveCamera();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Position Offset");
		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
		ImGui::DragFloat3("##fpsCameraPositionOffset", (float*)&camera->GetPositionOffset(), 1, -10000, 10000,
						  "%.1f");

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Rotation Offset");
		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
		ImGui::DragFloat3("##fpsCameraRotationOffset", (float*)&camera->GetRotationOffset(), 1, -180, 180,
						  "%.1f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Field of View");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
        if (ImGui::SliderFloat("##fovSlider", &fov, 45, 160, "%.1f"))
        {
            Mod::GetGameInterface()->SetFov(fov);
        }
    }

    void DrawThirdPersonSettings()
    {
		auto columnPercent = 0.4f;

        auto& campathManager = Components::CampathManager::Get();
        auto& cameraManager = Components::CameraManager::Get();
        auto& camera = cameraManager.GetActiveCamera();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Position Offset");
		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
		ImGui::DragFloat3("##fpsCameraPositionOffset", (float*)&camera->GetPositionOffset(), 1, -10000, 10000,
						  "%.1f");

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Rotation Offset");
		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
		ImGui::DragFloat3("##fpsCameraRotationOffset", (float*)&camera->GetRotationOffset(), 1, -180, 180,
						  "%.1f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Field of View");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
        if (ImGui::SliderFloat("##fovSlider", &fov, 45, 160, "%.1f"))
        {
            Mod::GetGameInterface()->SetFov(fov);
        }

    }

    void DrawFreeCameraSettings()
    {
		auto columnPercent = 0.4f;

        auto& campathManager = Components::CampathManager::Get();
        auto& cameraManager = Components::CameraManager::Get();
        auto& camera = cameraManager.GetActiveCamera();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Position");
		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
		ImGui::DragFloat3("##fpsCameraPositionOffset", (float*)&camera->GetPosition(), 1, -10000, 10000,
						  "%.1f");

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Rotation");
		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
		ImGui::DragFloat3("##fpsCameraRotationOffset", (float*)&camera->GetRotation(), 1, -180, 180,
						  "%.1f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Field of View");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
        if (ImGui::SliderFloat("##fovSlider", &fov, 45, 160, "%.1f"))
        {
            Mod::GetGameInterface()->SetFov(fov);
        }
    }

    void DrawBoneCameraSettings()
    {
        auto columnPercent = 0.4f;

		auto& campathManager = Components::CampathManager::Get();
        auto& cameraManager = Components::CameraManager::Get();
        auto& camera = cameraManager.GetActiveCamera();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Position Offset");
		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
		ImGui::DragFloat3("##boneCameraPositionOffset", (float*)&camera->GetPositionOffset(), 1, -10000, 10000,
						  "%.1f");

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Rotation Offset");
		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
		ImGui::DragFloat3("##boneCameraRotationOffset", (float*)&camera->GetRotationOffset(), 1, -180, 180,
						  "%.1f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Entity");
        ImGui::SameLine();

        auto boneCamera = static_cast<Components::BoneCamera*>(camera.get());
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
        auto entities = Mod::GetGameInterface()->GetEntities();
        if (ImGui::BeginCombo("##gameViewBoneCameraTargetCombo",
                              entities[boneCamera->GetEntityId()].ToString().c_str()))
        {
            for (size_t i = 0; i < entities.size(); i++)
            {
                if (entities[i].type == Types::EntityType::Unsupported)
                    continue;

                bool isSelected = boneCamera->GetEntityId() == i;
                if (ImGui::Selectable(entities[i].ToString().c_str(), boneCamera->GetEntityId() == i))
                {
                    boneCamera->GetEntityId() = i;
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Bone");
        ImGui::SameLine();

        const auto& supportedBoneNames = Mod::GetGameInterface()->GetSupportedBoneNames();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
        if (ImGui::BeginCombo("##gameViewBoneCameraBoneCombo", supportedBoneNames[boneCamera->GetBoneIndex()].c_str()))
        {
            for (size_t i = 0; i < supportedBoneNames.size(); i++)
            {
                bool isSelected = boneCamera->GetBoneIndex() == i;
                if (ImGui::Selectable(supportedBoneNames[i].data(), boneCamera->GetBoneIndex() == i))
                {
                    boneCamera->GetBoneIndex() = i;
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }


        const auto& boneData = Mod::GetGameInterface()->GetBoneData(boneCamera->GetEntityId(),
                                                                    supportedBoneNames.at(boneCamera->GetBoneIndex()));
        if (boneData.id == -1)
        {
            ImGui::Dummy(ImVec2(0, 10));

            ImGui::Text(ICON_FA_TRIANGLE_EXCLAMATION " Bone not found on entity");
        }
        else
        {
            ImGui::Dummy(ImVec2(0, 10));

			ImGui::AlignTextToFramePadding();
            ImGui::Text("Field of View");
            ImGui::SameLine();

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
            ImGui::DragFloat("##gameViewBoneCameraFOV", &boneCamera->GetFov(), 1, 1, 180, "%.0f");

            ImGui::BeginDisabled();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Smooth Movement");
            ImGui::SameLine();

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
            ImGui::Checkbox("##gameViewBoneCameraTemporalSmoothing", &boneCamera->UseTemporalSmoothing());
            ImGui::EndDisabled();

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Show Bone");
            ImGui::SameLine();

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) -
                                    ImGui::GetStyle().WindowPadding.x);
            ImGui::Checkbox("##gameViewBoneCameraShowBone", &boneCamera->ShowBone());
        }
    }

    void DrawNoSettings()
    {
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::TextWrapped("There are no settings for this camera mode!");
    }

    void DrawDollycamSettings()
    {
        auto columnPercent = 0.4f;

        auto& keyframeManager = Components::KeyframeManager::Get();
        const auto& property = keyframeManager.GetProperty(Types::KeyframeablePropertyType::CampathCamera);
        const auto& campathNodes = keyframeManager.GetKeyframes(property);

        if (campathNodes.empty())
        {
            auto& config = InputConfiguration::Get();

            ImGui::TextWrapped(
                "No campath nodes set! To create a dollycam campath, go into Free Camera and press %s to place nodes.",
                ImGui::GetKeyName(config.GetBoundKey(Action::DollyAddNode)));
            return;
        }

        ImGui::Text("Placed Nodes");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
        ImGui::Text("%d", campathNodes.size());

        ImGui::Text("Interpolation");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
        ImGui::Text("%s", campathNodes.size() < 4 ? "Linear" : "Cubic");

        ImGui::Dummy(ImVec2(0, 5));

        if (campathNodes.size() < 4)
        {
            ImGui::TextWrapped("You've placed less than 4 nodes. Your campath will use linear interpolation.");
        }
    }

    void DrawShakeSection()
    {
        auto& shakeManager = Components::ShakeManager::Get();

        for (size_t i = 0; i < shakeManager.shakes.size(); i++)
        {
            ImGui::PushID(std::format("##shake{}", i).c_str());

            Types::KeyframeablePropertyType keyframePropertyType = {};
            switch (i)
            {
                case 0:
                    keyframePropertyType = Types::KeyframeablePropertyType::Shake1;
                    break;
                case 1:
                    keyframePropertyType = Types::KeyframeablePropertyType::Shake2;
                    break;
                case 2:
                    keyframePropertyType = Types::KeyframeablePropertyType::Shake3;
                    break;
                case 3:
                    keyframePropertyType = Types::KeyframeablePropertyType::Shake4;
                    break;
            }

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Shake %d", i);

            auto& it = shakeManager.shakes[i];
            ImGuiEx::Keyframeable::SliderFloat("Amplitude", &it.amplitude, 0.0f, 1.0f, keyframePropertyType);
            ImGui::SliderFloat("Frequency", &it.frequency, 0.001f, 10.0f);

            ImGui::SliderFloat("Forwards", &it.forwards, 0.0f, 50.0f);
            ImGui::SliderFloat("Sideways", &it.sideways, 0.0f, 50.0f);
            ImGui::SliderFloat("Vertical", &it.vertical, 0.0f, 50.0f);

            ImGui::SliderFloat("Yaw", &it.yaw, 0.0f, 45.0f);
            ImGui::SliderFloat("Pitch", &it.pitch, 0.0f, 45.0f);
            ImGui::SliderFloat("Roll", &it.roll, 0.0f, 45.0f);

            ImGui::SliderInt("Seed", &it.seed, 0, 10000);

            ImGui::PopID();
        }
    }

    void CameraMenu::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("Camera", NULL, flags);

        if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
        {
            UI::DrawInaccessibleTabWarning();
            ImGui::End();
            return;
        }

        auto& campathManager = Components::CampathManager::Get();
        auto& cameraManager = Components::CameraManager::Get();
        auto& camera = cameraManager.GetActiveCamera();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::PushFont(UIManager::Get().GetBoldFont());
        ImGui::Text(ICON_FA_CAMERA "  %s Settings", cameraManager.GetCameraModeLabel(camera->GetMode()));
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        switch (camera->GetMode())
        {
			case Components::Camera::Mode::FirstPerson:
                DrawFirstPersonSettings();
                break;
			case Components::Camera::Mode::ThirdPerson:
				DrawThirdPersonSettings();
				break;
			case Components::Camera::Mode::Free:
				DrawFreeCameraSettings();
				break;
            case Components::Camera::Mode::Bone:
                DrawBoneCameraSettings();
                break;
            case Components::Camera::Mode::Dolly:
                DrawDollycamSettings();
                break;
            default:
                DrawNoSettings();
                break;
        }

        DrawShakeSection();

        ImGui::End();
    }

    void CameraMenu::Release()
    {
    }
}  // namespace IWXMVM::UI