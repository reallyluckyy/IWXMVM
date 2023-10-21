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

        Types::Keyframe Interpolate(const Types::KeyframeableProperty& property, const unsigned int& tick) const;

       private:
        KeyframeManager()
        {
        }

        std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>> keyframes;
    };
}  // namespace IWXMVM::Components