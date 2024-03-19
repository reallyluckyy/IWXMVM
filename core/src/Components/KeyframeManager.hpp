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

        struct UndoAction
        {
            Types::KeyframeableProperty property;
            virtual ~UndoAction(){}

            UndoAction(const Types::KeyframeableProperty& prop) : property(prop){}
        };

        struct AddAction : UndoAction
        {
            Types::Keyframe keyframeToAdd;

            AddAction(const Types::KeyframeableProperty& prop, const Types::Keyframe& keyframe)
                : UndoAction(prop), keyframeToAdd(keyframe){}
        };

        struct RemoveAction : UndoAction
        {
            Types::Keyframe keyframeToRemove;

            RemoveAction(const Types::KeyframeableProperty& prop, const Types::Keyframe& keyframe)
                : UndoAction(prop), keyframeToRemove(keyframe){}
        };

        void Undo();

        void AddKeyframe(Types::KeyframeableProperty property, Types::Keyframe keyframeToAdd);

        void RemoveKeyframe(Types::KeyframeableProperty property, size_t indexToRemove);
        void RemoveKeyframe(Types::KeyframeableProperty property, Types::Keyframe keyframeToRemove);

        Types::KeyframeValue Interpolate(const Types::KeyframeableProperty& property,
                                         const std::vector<Types::Keyframe>& keyframes, const float tick) const;
        Types::KeyframeValue Interpolate(const Types::KeyframeableProperty& property,
                                         const std::vector<Types::Keyframe>& keyframes, const uint32_t tick) const; 
        Types::KeyframeValue Interpolate(const Types::KeyframeableProperty& property, const float tick) const;
        Types::KeyframeValue Interpolate(const Types::KeyframeableProperty& property, const uint32_t tick) const;

        const Types::KeyframeableProperty& GetProperty(const Types::KeyframeablePropertyType property) const;

        void ClearKeyframes();

        void SortAndSaveKeyframes(std::vector<Types::Keyframe>& keyframes);

       private:
        KeyframeManager()
        {
        }

        Types::KeyframeValue CubicInterpolate(Types::KeyframeValueType valueType, const auto& keyframes,
                                              const float tick) const;
        Types::KeyframeValue LinearlyInterpolate(Types::KeyframeValueType valueType, const auto& keyframes,
                                                 const float tick) const;

        std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>> keyframes;
        std::vector<UndoAction*> undoActions;
    };
}  // namespace IWXMVM::Components