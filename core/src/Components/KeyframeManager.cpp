#include "StdInclude.hpp"
#include "KeyframeManager.hpp"

#include "Resources.hpp"
#include "Utilities/MathUtils.hpp"

namespace IWXMVM::Components
{
    Types::KeyframeableProperty campathCameraProperty(ICON_FA_VIDEO " Campath Camera",
                                                        Types::KeyframeValueType::CameraData);
    Types::KeyframeableProperty sunLightBrightnessProperty(ICON_FA_SUN " Sun Light Brightness",
                                             Types::KeyframeValueType::FloatingPoint);

    void KeyframeManager::Initialize()
    {
        auto InitializeProperty = [&](auto property) { keyframes[property] = std::vector<Types::Keyframe>(); }; 
        
        InitializeProperty(campathCameraProperty);
        InitializeProperty(sunLightBrightnessProperty);
    }

    const Types::KeyframeableProperty& KeyframeManager::GetProperty(const Types::KeyframeablePropertyType property) const
    {
        switch (property)
        {
            case Types::KeyframeablePropertyType::CampathCamera:
                return campathCameraProperty;
            case Types::KeyframeablePropertyType::SunLightBrightness:
                return sunLightBrightnessProperty;
        }
    }

    Types::KeyframeValue KeyframeManager::Interpolate(const Types::KeyframeableProperty& property,
                                                      const std::vector<Types::Keyframe>& keyframes,
                                                      const float tick) const
    {
        if (keyframes.empty())
            return Types::KeyframeValue::GetDefaultValue(property.valueType);

        if (tick < keyframes.front().tick)
            return keyframes.front().value;
        if (tick > keyframes.back().tick)
            return keyframes.back().value;

        // TODO: interpolation selection in the future
        if (keyframes.size() < 4)
            return LinearlyInterpolate(property.valueType, keyframes, tick);

        return CubicInterpolate(property.valueType, keyframes, tick);
    }

    Types::KeyframeValue KeyframeManager::Interpolate(const Types::KeyframeableProperty& property, const float tick) const
    {
        const auto& keyframes = KeyframeManager::Get().GetKeyframes(property);
        return Interpolate(property, keyframes, tick);
    }

    Types::KeyframeValue KeyframeManager::CubicInterpolate(Types::KeyframeValueType valueType,
                                                           const auto& keyframes,
                                                           const float tick) const
    {
        switch (valueType)
        {
            case Types::KeyframeValueType::FloatingPoint:
                return MathUtils::InterpolateCubicSpline(keyframes, 0, tick);
            case Types::KeyframeValueType::Vector3:
                return glm::vec3(
                    MathUtils::InterpolateCubicSpline(keyframes, 0, tick),
                    MathUtils::InterpolateCubicSpline(keyframes, 1, tick),
                    MathUtils::InterpolateCubicSpline(keyframes, 2, tick)
                );
            case Types::KeyframeValueType::CameraData:
            {
                return Types::KeyframeValue(
                    Types::CameraData(
                        glm::vec3(
                            MathUtils::InterpolateCubicSpline(keyframes, 0, tick),
                            MathUtils::InterpolateCubicSpline(keyframes, 1, tick),
                            MathUtils::InterpolateCubicSpline(keyframes, 2, tick)
                        ),
                        // TODO: do proper quaternion interpolation here
                        glm::vec3(
                            MathUtils::InterpolateCubicSpline(keyframes, 3, tick),
                            MathUtils::InterpolateCubicSpline(keyframes, 4, tick),
                            MathUtils::InterpolateCubicSpline(keyframes, 5, tick)
                        ), 
                        MathUtils::InterpolateCubicSpline(keyframes, 6, tick)
                    )
                );
            }
            default:
                return Types::KeyframeValue(0.0f);
        }
    }

    Types::KeyframeValue KeyframeManager::LinearlyInterpolate(Types::KeyframeValueType valueType, const auto& keyframes,
                                                              const float tick) const
    {
        if (keyframes.size() == 1)
			return keyframes.front().value;

        std::int32_t p0Idx = 0, p1Idx = 1;
        for (std::size_t i = 0; i < keyframes.size() - 1; i++)
        {
            if (tick >= keyframes[i].tick && tick <= keyframes[i + 1].tick)
            {
                p0Idx = i;
                p1Idx = i + 1;
                break;
            }
        }

        assert(p0Idx != -1 || p1Idx != -1);
        assert(keyframes[p0Idx].tick < keyframes[p1Idx].tick);
        assert(p0Idx != p1Idx);

        const auto& p0 = keyframes[p0Idx];
        const auto& p1 = keyframes[p1Idx];

        const float t = static_cast<float>(tick - p0.tick) / static_cast<float>(p1.tick - p0.tick);
        switch (valueType)
        {
            case Types::KeyframeValueType::FloatingPoint:
                return Types::KeyframeValue((1.0f - t) * p0.value.floatingPoint + t * p1.value.floatingPoint);
            case Types::KeyframeValueType::Vector3:
                return Types::KeyframeValue((1.0f - t) * p0.value.vector3 + t * p1.value.vector3);
            case Types::KeyframeValueType::CameraData:
                return Types::KeyframeValue(
                    Types::CameraData((1.0f - t) * p0.value.cameraData.position + t * p1.value.cameraData.position,
                                      (1.0f - t) * p0.value.cameraData.rotation + t * p1.value.cameraData.rotation,
                                      (1.0f - t) * p0.value.cameraData.fov + t * p1.value.cameraData.fov));
            default:
                return Types::KeyframeValue(0.0f);
        }
    }

}  // namespace IWXMVM::Components
