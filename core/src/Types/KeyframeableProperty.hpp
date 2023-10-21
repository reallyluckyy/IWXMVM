#pragma once

namespace IWXMVM::Types
{
    enum class KeyframeValueType
    {
        FloatingPoint,
		Vector3
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