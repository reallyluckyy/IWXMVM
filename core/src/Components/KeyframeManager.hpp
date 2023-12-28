#pragma once
#include "Types/Keyframe.hpp"
#include "Types/KeyframeableProperty.hpp"

namespace IWXMVM::Components
{
    class KeyframeManager
    {
       public:
        static KeyframeManager& Get()
        {
            static KeyframeManager instance;
            return instance;
        }

        KeyframeManager(KeyframeManager const&) = delete;
        void operator=(KeyframeManager const&) = delete;

        void Initialize();
        const std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>>& GetKeyframes() const
        {
            return keyframes;
        }

        std::vector<Types::Keyframe>& GetKeyframes(const Types::KeyframeableProperty& property)
        {
            return keyframes[property];
        }

        Types::KeyframeValue Interpolate(const Types::KeyframeableProperty& property,
                                         const std::vector<Types::Keyframe>& keyframes, const float tick) const;
        Types::KeyframeValue Interpolate(const Types::KeyframeableProperty& property, const float tick) const;

        const Types::KeyframeableProperty& GetProperty(const Types::KeyframeablePropertyType property) const;

        void ClearKeyframes();

       private:
        KeyframeManager()
        {
        }

        Types::KeyframeValue CubicInterpolate(Types::KeyframeValueType valueType, const auto& keyframes,
                                              const float tick) const;
        Types::KeyframeValue LinearlyInterpolate(Types::KeyframeValueType valueType, const auto& keyframes,
                                                 const float tick) const;

        std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>> keyframes;
    };
}  // namespace IWXMVM::Components