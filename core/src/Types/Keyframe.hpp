#pragma once
#include "KeyframeableProperty.hpp"

namespace IWXMVM::Types
{
    struct CameraData
    {
        glm::vec3 position;
        glm::vec3 rotation;
        float fov;
        bool operator==(const CameraData& other) const
        {
            return position == other.position && rotation == other.rotation && fov == other.fov;
        }
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

        static KeyframeValue GetDefaultValue(Types::KeyframeValueType valueType)
        {
            switch (valueType)
            {
                case Types::KeyframeValueType::FloatingPoint:
					return KeyframeValue(0.0f);
                case Types::KeyframeValueType::Vector3:
					return KeyframeValue(glm::vec3(0.0f));
                case Types::KeyframeValueType::CameraData:
					return KeyframeValue(CameraData(glm::vec3(0.0f), glm::vec3(0.0f), 0.0f));
				default:
					throw std::invalid_argument("Invalid valueType");
			}
		}

        static std::string_view GetValueIndexName(Types::KeyframeValueType valueType, uint32_t index)
        {
            switch (valueType)
            {
				case Types::KeyframeValueType::FloatingPoint:
					return "Value";
				case Types::KeyframeValueType::Vector3:
                    switch (index)
                    {
						case 0:
							return "X";
						case 1:
							return "Y";
						case 2:
							return "Z";
						default:
							throw std::invalid_argument("Invalid index for Vector3");
					}
				case Types::KeyframeValueType::CameraData:
                    switch (index)
                    {
						case 0:
							return "X";
						case 1:
							return "Y";
						case 2:
							return "Z";
                        case 3:
                            return "Pitch";
                        case 4:
                            return "Yaw";
                        case 5:
                            return "Roll";
                        case 6:
                            return "FOV";
						default:
							throw std::invalid_argument("Invalid index for CameraData");
					}
				default:
					throw std::invalid_argument("Invalid valueType");
			}
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