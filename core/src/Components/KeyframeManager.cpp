#include "StdInclude.hpp"
#include "KeyframeManager.hpp"

#include "Resources.hpp"

namespace IWXMVM::Components
{
    Types::KeyframeableProperty campathCameraProperty(ICON_FA_VIDEO " Campath Camera",
                                                        Types::KeyframeValueType::CameraData);
    Types::KeyframeableProperty testProperty(ICON_FA_CAMPGROUND " Test Property",
                                             Types::KeyframeValueType::FloatingPoint);

    void KeyframeManager::Initialize()
    {
        keyframes[campathCameraProperty] = std::vector<Types::Keyframe>();
        keyframes[testProperty] = std::vector<Types::Keyframe>();
    }

    const Types::KeyframeableProperty& KeyframeManager::GetProperty(const Types::KeyframeablePropertyType property) const
    {
        switch (property)
        {
            case Types::KeyframeablePropertyType::CampathCamera:
                return campathCameraProperty;
            case Types::KeyframeablePropertyType::TestProperty:
                return testProperty;
        }
    }

    Types::Keyframe KeyframeManager::Interpolate(const Types::KeyframeableProperty& property, const uint32_t tick) const
    {
        const auto& keyframes = KeyframeManager::Get().GetKeyframes(property);

        std::int32_t p0Idx = -1, p1Idx = -1;
        for (std::size_t i = 0; i < keyframes.size(); i++)
        {
            if (keyframes[i].tick < tick && (p0Idx == -1 || keyframes [i].tick > keyframes[p0Idx].tick))
				p0Idx = i;
            if (keyframes[i].tick > tick && (p1Idx == -1 || keyframes[i].tick < keyframes[p1Idx].tick))
                p1Idx = i;

            if (keyframes[i].tick == tick)
				return keyframes[i];
        }

        if (p0Idx == -1)
			return Types::Keyframe(property, tick, keyframes[p1Idx].value);

        if (p1Idx == -1)
            return Types::Keyframe(property, tick, keyframes[p0Idx].value);

        assert(p0Idx != -1 || p1Idx != -1);
        assert(keyframes[p0Idx].tick < keyframes[p1Idx].tick);
        assert(p0Idx != p1Idx);

        const auto& p0 = keyframes[p0Idx];
        const auto& p1 = keyframes[p1Idx];

        const float t = static_cast<float>(tick - p0.tick) / static_cast<float>(p1.tick - p0.tick);
        switch (property.valueType)
        {
            case Types::KeyframeValueType::FloatingPoint:
                return Types::Keyframe(property, tick, (1.0f - t) * p0.value.floatingPoint + t * p1.value.floatingPoint);
            case Types::KeyframeValueType::Vector3:
                return Types::Keyframe(property, tick, (1.0f - t) * p0.value.vector3 + t * p1.value.vector3);
            case Types::KeyframeValueType::CameraData:
                return Types::Keyframe(property, tick, 
                    Types::CameraData(
                        (1.0f - t) * p0.value.cameraData.position + t * p1.value.cameraData.position,
                        (1.0f - t) * p0.value.cameraData.rotation + t * p1.value.cameraData.rotation,
                        (1.0f - t) * p0.value.cameraData.fov + t * p1.value.cameraData.fov
                    )
                );
            default:
                return Types::Keyframe(property, tick, 0.0f);
        }
    }

}  // namespace IWXMVM::Components
