#include "StdInclude.hpp"
#include "KeyframeManager.hpp"

#include "Resources.hpp"
#include "Utilities/MathUtils.hpp"
#include "KeyframeSerializer.hpp"
#include "Events.hpp"

namespace IWXMVM::Components
{
    Types::KeyframeableProperty campathCameraProperty(
        Types::KeyframeablePropertyType::CampathCamera, 
        ICON_FA_VIDEO " Campath Camera",
        Types::KeyframeValueType::CameraData,
        -3000, 3000 // TODO: this is not super helpful for rotation and fov
    );
    Types::KeyframeableProperty sunLightColorProperty(
        Types::KeyframeablePropertyType::SunLightColor,
        ICON_FA_SUN " Sun Light Color",
        Types::KeyframeValueType::Vector3,
        0, 255
    );
    Types::KeyframeableProperty sunLightBrightnessProperty(
        Types::KeyframeablePropertyType::SunLightBrightness,
        ICON_FA_SUN " Sun Light Brightness",
        Types::KeyframeValueType::FloatingPoint,
        0, 4
    );
    Types::KeyframeableProperty sunLightPitchProperty(
        Types::KeyframeablePropertyType::SunLightPitch,
        ICON_FA_SUN " Sun Light Pitch",
        Types::KeyframeValueType::FloatingPoint,
        0, 360
    );
    Types::KeyframeableProperty sunLightYawProperty(
        Types::KeyframeablePropertyType::SunLightYaw,
        ICON_FA_SUN " Sun Light Yaw",
        Types::KeyframeValueType::FloatingPoint, 
        0, 360
    );
    Types::KeyframeableProperty filmtweakBrightnessProperty(
        Types::KeyframeablePropertyType::FilmtweakBrightness,
        ICON_FA_IMAGE " Filmtweak Brightness",
        Types::KeyframeValueType::FloatingPoint,
        -1, 1
    );
    Types::KeyframeableProperty filmtweakContrastProperty(
        Types::KeyframeablePropertyType::FilmtweakContrast,
        ICON_FA_IMAGE " Filmtweak Contrast",
        Types::KeyframeValueType::FloatingPoint,
        0, 4
    );
    Types::KeyframeableProperty filmtweakDesaturationProperty(
        Types::KeyframeablePropertyType::FilmtweakDesaturation,
        ICON_FA_IMAGE " Filmtweak Desaturation",
        Types::KeyframeValueType::FloatingPoint, 
        0, 1
    );
    Types::KeyframeableProperty filmtweakTintLightProperty(
        Types::KeyframeablePropertyType::FilmtweakTintLight,
        ICON_FA_IMAGE " Filmtweak Tint Light",
        Types::KeyframeValueType::Vector3,
        0, 255
    );
    Types::KeyframeableProperty filmtweakTintDarkProperty(
        Types::KeyframeablePropertyType::FilmtweakTintDark,
        ICON_FA_IMAGE " Filmtweak Tint Dark",
        Types::KeyframeValueType::Vector3,
        0, 255
    );
    Types::KeyframeableProperty dofFarBlur(
        Types::KeyframeablePropertyType::DepthOfFieldFarBlur,
        ICON_FA_CAMERA " DOF Far Blur",
        Types::KeyframeValueType::FloatingPoint,
        0, 10
    );
    Types::KeyframeableProperty dofFarStart(
        Types::KeyframeablePropertyType::DepthOfFieldFarStart,
        ICON_FA_CAMERA " DOF Far Start",
        Types::KeyframeValueType::FloatingPoint, 
        0, 5000
    );
    Types::KeyframeableProperty dofFarEnd(
        Types::KeyframeablePropertyType::DepthOfFieldFarEnd,
        ICON_FA_CAMERA " DOF Far End", 
        Types::KeyframeValueType::FloatingPoint, 
        0, 5000
    );
    Types::KeyframeableProperty dofNearBlur(
        Types::KeyframeablePropertyType::DepthOfFieldNearBlur,
        ICON_FA_CAMERA " DOF Near Blur", 
        Types::KeyframeValueType::FloatingPoint,
        0, 10
    );
    Types::KeyframeableProperty dofNearStart(
        Types::KeyframeablePropertyType::DepthOfFieldNearStart,
        ICON_FA_CAMERA " DOF Near Start", 
        Types::KeyframeValueType::FloatingPoint, 
        0, 5000
    );
    Types::KeyframeableProperty dofNearEnd(
        Types::KeyframeablePropertyType::DepthOfFieldNearEnd,
        ICON_FA_CAMERA " DOF Near End", 
        Types::KeyframeValueType::FloatingPoint,
        0, 5000
    );
    Types::KeyframeableProperty dofBias(
        Types::KeyframeablePropertyType::DepthOfFieldBias,
        ICON_FA_CAMERA " DOF Bias", 
        Types::KeyframeValueType::FloatingPoint, 
        0.1f, 10
    );

    void KeyframeManager::Initialize()
    {
        auto InitializeProperty = [&](auto property) { keyframes[property] = std::vector<Types::Keyframe>(); }; 
        
        InitializeProperty(campathCameraProperty);
        InitializeProperty(sunLightColorProperty);
        InitializeProperty(sunLightBrightnessProperty);
        InitializeProperty(sunLightPitchProperty);
        InitializeProperty(sunLightYawProperty);
        InitializeProperty(filmtweakBrightnessProperty);
        InitializeProperty(filmtweakContrastProperty);
        InitializeProperty(filmtweakDesaturationProperty);
        InitializeProperty(filmtweakTintLightProperty);
        InitializeProperty(filmtweakTintDarkProperty);
        InitializeProperty(dofFarBlur);
        InitializeProperty(dofFarStart);
        InitializeProperty(dofFarEnd);
        InitializeProperty(dofNearBlur);
        InitializeProperty(dofNearStart);
        InitializeProperty(dofNearEnd);
        InitializeProperty(dofBias);

        Events::RegisterListener(EventType::PostDemoLoad, [&]() { 
            ClearKeyframes();
            Components::KeyframeSerializer::ReadRecent();
        });
    }

    const Types::KeyframeableProperty& KeyframeManager::GetProperty(const Types::KeyframeablePropertyType property) const
    {
        for (auto& [p, _] : keyframes)
        {
            if (p.type == property)
                return p;
        }

        throw std::runtime_error("Unregistered keyframeable property type");
    }

    void KeyframeManager::ClearKeyframes()
    {
        for (auto& [p, k] : keyframes)
        {
            k.clear();
        }
    }

    void KeyframeManager::SortAndSaveKeyframes(std::vector<Types::Keyframe>& keyframes)
    {
        std::sort(keyframes.begin(), keyframes.end(), [](const auto& a, const auto& b) { return a.tick < b.tick; });

        Components::KeyframeSerializer::WriteRecent();
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

    Types::KeyframeValue KeyframeManager::Interpolate(const Types::KeyframeableProperty& property,
                                                      const std::vector<Types::Keyframe>& keyframes,
                                                      const uint32_t tick) const 
    {
        return Interpolate(property, keyframes, static_cast<float>(tick));
    }

    Types::KeyframeValue KeyframeManager::Interpolate(const Types::KeyframeableProperty& property, const float tick) const
    {
        const auto& keyframes = KeyframeManager::Get().GetKeyframes(property);
        return Interpolate(property, keyframes, tick);
    }

    Types::KeyframeValue KeyframeManager::Interpolate(const Types::KeyframeableProperty& property, const uint32_t tick) const 
    {
        return Interpolate(property, static_cast<float>(tick));
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
        assert(p0Idx != p1Idx);
        if (!(keyframes[p0Idx].tick < keyframes[p1Idx].tick))
        {
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
    }

}  // namespace IWXMVM::Components
