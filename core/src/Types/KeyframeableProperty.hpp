#pragma once

namespace IWXMVM::Types
{
    enum class KeyframeablePropertyType
    {
        CampathCamera,
        SunLightColor,
        SunLightBrightness,
        SunLightPitch,
        SunLightYaw
    };

    enum class KeyframeValueType
    {
        FloatingPoint,
        Vector3,
        CameraData
    };

    struct KeyframeableProperty
    {
        std::string_view name;
        KeyframeValueType valueType;

        KeyframeableProperty(std::string_view name, KeyframeValueType valueType)
            : name(name), valueType(valueType)
        {
        }

        bool operator<(const KeyframeableProperty& other) const
        {
            return name < other.name;
        }
    };
}