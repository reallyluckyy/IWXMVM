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
        if (Rewinding::IsRewinding() )
            return;

        auto& keyframeManager = KeyframeManager::Get();
        const auto& property = keyframeManager.GetProperty(Types::KeyframeablePropertyType::CampathCamera);

        if (keyframeManager.GetKeyframes(property).empty())
            return;

        const auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
        const auto interpolatedValue = keyframeManager.Interpolate(property, currentTick);

        this->GetPosition() = interpolatedValue.cameraData.position;
        this->GetRotation() = interpolatedValue.cameraData.rotation;
        this->GetFov() = interpolatedValue.cameraData.fov;
    }
}  // namespace IWXMVM::Components
