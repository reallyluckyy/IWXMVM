#include "StdInclude.hpp"
#include "DollyCamera.hpp"

#include "Mod.hpp"
#include "Components/Rewinding.hpp"

namespace IWXMVM::Components
{
    void DollyCamera::Initialize()
    {
    }

    void DollyCamera::Update()
    {
        if (Rewinding::IsRewinding())
            return;
        const auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
        
        const auto& keyframeManager = KeyframeManager::Get();
        const auto interpolatedValue = keyframeManager.Interpolate(
            keyframeManager.GetProperty(Types::KeyframeablePropertyType::CampathCamera), currentTick);

        this->GetPosition() = interpolatedValue.cameraData.position;
        this->GetRotation() = interpolatedValue.cameraData.rotation;
        this->GetFov() = interpolatedValue.cameraData.fov;
    }
}  // namespace IWXMVM::Components
