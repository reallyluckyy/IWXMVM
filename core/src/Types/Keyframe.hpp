#pragma once
#include "KeyframeableProperty.hpp"

namespace IWXMVM::Types
{
    union KeyframeValue
    {
        float floating_point;
    	glm::vec3 vector3;

        KeyframeValue(float floating_point)
			: floating_point(floating_point)
        {
        }

        KeyframeValue(glm::vec3 vector3)
            : vector3(vector3)
        {
		}
    };

    struct Keyframe
    {
        const KeyframeableProperty& property;

        std::uint32_t tick;
        KeyframeValue value;

        Keyframe(const KeyframeableProperty& property, std::uint32_t tick, KeyframeValue value)
            : property(property), tick(tick), value(value)
        {
		}

        Keyframe(const KeyframeableProperty& property)
			: property(property), tick(0), value(0)
		{}

        Keyframe operator=(const Keyframe& other)
        {
			tick = other.tick;
			value = other.value;
			return *this;
        }

        bool operator<(const Keyframe& other) const
        {
			return tick < other.tick;
        }
    };
}