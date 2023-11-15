#include "StdInclude.hpp"
#include "DollyCamera.hpp"

#include "Mod.hpp"

namespace IWXMVM::Components
{
    void DollyCamera::Initialize()
    {
    }

    void DollyCamera::Update()
    {
        const auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
        
        const auto& keyframeManager = KeyframeManager::Get();
        const auto interpolatedKeyframe = keyframeManager.Interpolate(
            keyframeManager.GetProperty(Types::KeyframeablePropertyType::CampathCamera), currentTick);

        this->GetPosition() = interpolatedKeyframe.value.cameraData.position;
        this->GetRotation() = interpolatedKeyframe.value.cameraData.rotation;
        this->GetFov() = interpolatedKeyframe.value.cameraData.fov;
    }
}  // namespace IWXMVM::Components
