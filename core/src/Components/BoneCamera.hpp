#pragma once
#include "Camera.hpp"
#include "Types/BoneData.hpp"

namespace IWXMVM::Components
{
    class BoneCamera : public Camera
    {
       public:
        BoneCamera()
        {
            this->mode = Camera::Mode::Bone;
            entityId = 0;
            boneIndex = 0;
            positionOffset = glm::vec3(0);
            rotationOffset = glm::vec3(0);
            useTemporalSmoothing = false;
            showBone = true;
        }

        void Initialize() override;
        void Update() override;

        int32_t& GetEntityId() { return entityId; }
        int32_t& GetBoneIndex() { return boneIndex; }
        glm::vec3& GetPositionOffset()
        {
            return positionOffset;
        }
        glm::vec3& GetRotationOffset()
        {
            return rotationOffset;
        }

        bool& UseTemporalSmoothing()
        {
            return useTemporalSmoothing;
        }

        bool& ShowBone()
        {
            return showBone;
        }
       
       private:
        int32_t entityId;
        int32_t boneIndex;

        glm::vec3 positionOffset;
        glm::vec3 rotationOffset;

        bool useTemporalSmoothing;
        bool showBone;

        void SetPositionFromBoneData(const Types::BoneData& boneData);
        void HandleInput(const Types::BoneData& boneData);
    };
}  // namespace IWXMVM::Components