#include "StdInclude.hpp"
#include "BoneCamera.hpp"

#include "Mod.hpp"
#include "Events.hpp"
#include "Components/FreeCamera.hpp"
#include "Utilities/MathUtils.hpp"
#include "UI/UIManager.hpp"

namespace IWXMVM::Components
{
    void BoneCamera::Initialize()
    {
        positionOffset = glm::vec3(0, 10, 0);

        Events::RegisterListener(EventType::PostDemoLoad, [&]() {
            entityId = 0;
        });
    }

    void BoneCamera::SetPositionFromBoneData(const Types::BoneData& boneData)
    {
        if (boneData.id == -1)
            return;

        this->position = boneData.position + boneData.rotation * positionOffset;

        auto qBoneRotation = glm::quat(boneData.rotation);
        auto qRotationOffset = glm::quat(glm::radians(rotationOffset));
        auto newRotation = glm::degrees(glm::eulerAngles(qBoneRotation * qRotationOffset));

        // im sure theres a glm way to fix our orientation without having
        // to do this manually (also in Graphics.cpp), but im just happy this works right now
        this->rotation[0] = newRotation[1];
        this->rotation[1] = newRotation[2];
        this->rotation[2] = newRotation[0];
    }

    void BoneCamera::HandleInput(const Types::BoneData& boneData)
    {
        if (!UI::UIManager::Get().GetUIComponent(UI::Component::GameView)->HasFocus())
            return;
        auto worldSpacePosition = boneData.position + boneData.rotation * positionOffset;
        FreeCamera::HandleFreecamInput(worldSpacePosition, rotationOffset, fov, GetForwardVector(), GetRightVector());
        positionOffset = boneData.rotation / (worldSpacePosition - boneData.position);
    }
    
    void BoneCamera::Update()
    {
        const auto& bones = Mod::GetGameInterface()->GetSupportedBoneNames();
        const std::string& selectedBoneName = bones.at(boneIndex);
        const auto selectedEntityId = entityId;
        const auto& boneData = Mod::GetGameInterface()->GetBoneData(selectedEntityId, selectedBoneName);

        auto entities = Mod::GetGameInterface()->GetEntities();
        auto selectedEntity = entities.at(selectedEntityId);
        if (!selectedEntity.isValid && selectedEntity.type == Types::EntityType::Player)
        {
            for (const auto& entity : entities)
            {
                if (entity.type == Types::EntityType::Corpse && entity.clientNum == selectedEntityId)
                {
                    SetPositionFromBoneData(Mod::GetGameInterface()->GetBoneData(entity.id, selectedBoneName));
                    return;
                }
            }
        }

        HandleInput(boneData);

        SetPositionFromBoneData(boneData);
    }

}  // namespace IWXMVM::Components
