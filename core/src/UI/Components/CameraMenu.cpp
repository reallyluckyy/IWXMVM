#include "StdInclude.hpp"
#include "CameraMenu.hpp"

#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "UI/ImGuiEx/ImGuiExtensions.hpp"
#include "Components/CameraManager.hpp"
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

        auto& cameraManager = Components::CameraManager::Get();
        auto& currentCamera = cameraManager.GetActiveCamera();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Entity");
        ImGui::SameLine();

        auto boneCamera = static_cast<Components::BoneCamera*>(currentCamera.get());
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
            ImGui::Text("Offset");
            ImGui::SameLine();

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
            ImGui::DragFloat3("##gameViewBoneCameraOffset", &boneCamera->GetPositionOffset().x, 1, -10000, 10000,
                              "%.1f");


            ImGui::AlignTextToFramePadding();
            ImGui::Text("Rotation");
            ImGui::SameLine();

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * columnPercent);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1.0f - columnPercent) - ImGui::GetStyle().WindowPadding.x);
            ImGui::DragFloat3("##gameViewBoneCameraRotation", &boneCamera->GetRotationOffset().x, 1, -180, 180, "%.1f");


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

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::PushFont(UIManager::Get().GetBoldFont());
        ImGui::Text(ICON_FA_CAMERA "  %s Settings",
                    cameraManager.GetCameraModeLabel(cameraManager.GetActiveCamera()->GetMode()));
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        switch (cameraManager.GetActiveCamera()->GetMode())
        {
            case Components::Camera::Mode::FirstPerson:
                DrawFirstPersonSettings();
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

        ImGui::End();
    }

    void CameraMenu::Release()
    {
    }
}  // namespace IWXMVM::UI