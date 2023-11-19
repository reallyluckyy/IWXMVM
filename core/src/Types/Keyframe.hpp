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
        float GetByIndex(uint32_t index) const
        {
            if (index > sizeof(KeyframeValue) / sizeof(float))
                throw std::invalid_argument("Index for KeyframeValue out of range");    
            return (&this->floatingPoint)[index];
        }

        void SetByIndex(uint32_t index, float value)
        {
            if (index > sizeof(KeyframeValue) / sizeof(float))
                throw std::invalid_argument("Index for KeyframeValue out of range");    
            (&this->floatingPoint)[index] = value;
        }
    };

    struct Keyframe
    {
        std::reference_wrapper<const KeyframeableProperty> property;

        std::int32_t id;
        std::uint32_t tick;
        KeyframeValue value;

        Keyframe(const KeyframeableProperty& property, std::uint32_t tick, KeyframeValue value)
            : id(nextId++), property(property), tick(tick), value(value)
        {
        }

        Keyframe(const KeyframeableProperty& property) : id(nextId++), property(property), tick(0), value(0)
        {}

        bool operator<(const Keyframe& other) const
        {
            return tick < other.tick;
        }

        private:
            inline static std::atomic_int32_t nextId = 0;
    };
}