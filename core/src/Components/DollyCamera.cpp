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
    const auto& markers = CampathManager::Get().GetMarkers();

    if (markers.size() == 0)
    {
        return;
    }

    const auto& firstMarker = markers.front();
    const auto& lastMarker = markers.back();

    const auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;

    if (currentTick <= firstMarker.tick)
    {
        this->GetPosition() = firstMarker.position;
        this->GetRotation() = firstMarker.rotation;
        this->GetFov() = firstMarker.fov;

        return;
    }
    else if (currentTick >= lastMarker.tick)
    {
        this->GetPosition() = lastMarker.position;
        this->GetRotation() = lastMarker.rotation;
        this->GetFov() = lastMarker.fov;

        return;
    }

    // If the function has not returned yet, we know currentTick is inbetween firstMarker.tick and lastMarker.tick

    // lerp
    {
        // We want to know the markers that the current tick is sandwiched between
        // p0 will be the marker before the current tick, p1 the marker after the current tick

        std::size_t p0Idx = 0, p1Idx = 1;
        for (std::size_t i = 0; i < markers.size() - 1; i++)
        {
            if (currentTick >= markers[i].tick && currentTick <= markers[i + 1].tick)
            {
                p0Idx = i;
                p1Idx = i + 1;
                break;
            }
        }

        const auto& p0 = markers[p0Idx];
        const auto& p1 = markers[p1Idx];

        // Interpolate between p0 and p1
        const float t = static_cast<float>(currentTick - p0.tick) / static_cast<float>(p1.tick - p0.tick);
        this->GetPosition() = (1.0f - t) * p0.position + t * p1.position;
        this->GetRotation() = (1.0f - t) * p0.rotation + t * p1.rotation;
        this->GetFov() = (1.0f - t) * p0.fov + t * p1.fov;
    }
}
}  // namespace IWXMVM::Components
