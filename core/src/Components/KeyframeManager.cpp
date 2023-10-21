#include "StdInclude.hpp"
#include "KeyframeManager.hpp"

#include "Resources.hpp"

namespace IWXMVM::Components
{
    Types::KeyframeableProperty campathPositionProperty(ICON_FA_VIDEO " Campath Position",
                                                        Types::KeyframeValueType::Vector3);
    Types::KeyframeableProperty testProperty(ICON_FA_CAMPGROUND " Test Property",
                                             Types::KeyframeValueType::FloatingPoint);

    void KeyframeManager::Initialize()
    {
        keyframes[campathPositionProperty] = std::vector<Types::Keyframe>();
        keyframes[testProperty] = std::vector<Types::Keyframe>();
    }

    Types::Keyframe KeyframeManager::Interpolate(const Types::KeyframeableProperty& property,
                                                 const unsigned int& tick) const
    {
        auto keyframes = KeyframeManager::Get().GetKeyframes(property);

        std::size_t p0Idx = 0, p1Idx = 1;
        for (std::size_t i = 0; i < keyframes.size() - 1; i++)
        {
            if (tick >= keyframes[i].tick && tick <= keyframes[i + 1].tick)
            {
                p0Idx = i;
                p1Idx = i + 1;
                break;
            }
        }

        const auto& p0 = keyframes[p0Idx];
        const auto& p1 = keyframes[p1Idx];

        const float t = static_cast<float>(tick - p0.tick) / static_cast<float>(p1.tick - p0.tick);
        switch (property.valueType)
        {
            case Types::KeyframeValueType::FloatingPoint:
                return Types::Keyframe(property, tick,
                                       (1.0f - t) * p0.value.floating_point + t * p1.value.floating_point);
            case Types::KeyframeValueType::Vector3:
                return Types::Keyframe(property, tick, (1.0f - t) * p0.value.vector3 + t * p1.value.vector3);
            default:
                return Types::Keyframe(property, tick, 0.0f);
        }
    }

}  // namespace IWXMVM::Components
