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

        Events::RegisterListener(EventType::OnDemoLoad, [&]() {
            entityId = 0;
        });
    }
    
    void BoneCamera::Update()
    {
        const auto& bones = Mod::GetGameInterface()->GetSupportedBoneNames();
        const std::string& selectedBoneName = bones.at(boneIndex);
        const auto selectedEntity = entityId;
        const auto& boneData = Mod::GetGameInterface()->GetBoneData(selectedEntity, selectedBoneName);
        if (boneData.id != -1)
        {
            this->position = boneData.position + boneData.rotation * positionOffset;
            
            auto euler =
                MathUtils::AnglesFromAxis(glm::normalize(boneData.rotation[0]), glm::normalize(boneData.rotation[1]),
                                          glm::normalize(boneData.rotation[2]));
            this->rotation = euler + rotationOffset;
        }
    }

}  // namespace IWXMVM::Components
