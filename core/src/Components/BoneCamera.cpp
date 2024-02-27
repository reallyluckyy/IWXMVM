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

        auto euler =
            MathUtils::AnglesFromAxis(glm::normalize(boneData.rotation[0]), glm::normalize(boneData.rotation[1]),
                                      glm::normalize(boneData.rotation[2]));
        this->rotation = euler + rotationOffset;
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

        SetPositionFromBoneData(boneData);
    }

}  // namespace IWXMVM::Components
