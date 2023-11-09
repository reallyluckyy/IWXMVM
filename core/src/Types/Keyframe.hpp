#pragma once
#include "KeyframeableProperty.hpp"

namespace IWXMVM::Types
{
    struct CameraData
    {
        glm::vec3 position;
        glm::vec3 rotation;
        float fov;
    };

    union KeyframeValue
    {
        float floatingPoint;
        glm::vec3 vector3;
        CameraData cameraData;

        KeyframeValue(float floatingPoint) : floatingPoint(floatingPoint)
        {
        }

        KeyframeValue(glm::vec3 vector3)
            : vector3(vector3)
        {
        }

        KeyframeValue(CameraData cameraData)
            : cameraData(cameraData)
        {
        }

        // This is real spooky
        float GetFloatValueByIndex(uint32_t index) const
        {
            try
            {
                if (index > sizeof(KeyframeValue) / sizeof(float))
                    throw std::invalid_argument("Index for KeyframeValue out of range");    
                return (&this->floatingPoint)[index];
            }
            catch (...)
            {
                LOG_DEBUG("this threw");
            }
        }

        void SetFloatValueByIndex(uint32_t index, float value)
        {
            try
            {
                if (index > sizeof(KeyframeValue) / sizeof(float))
                    throw std::invalid_argument("Index for KeyframeValue out of range");    
                (&this->floatingPoint)[index] = value;
            }
            catch (...)
            {
                LOG_DEBUG("this threw");
            }
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