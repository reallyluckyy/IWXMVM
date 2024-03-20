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

        void HandleInput();

        void Initialize();
        const std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>>& GetKeyframes() const
        {
            return keyframes;
        }

        std::vector<Types::Keyframe>& GetKeyframes(const Types::KeyframeableProperty& property)
        {
            return keyframes[property];
        }

        struct Action
        {
            Types::KeyframeableProperty property;
            Action(const Types::KeyframeableProperty& prop) : property(prop){}

            virtual void Undo(std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>>& keyframes) const = 0;
        };

        struct AddAction : Action
        {
            Types::Keyframe keyframe;

            AddAction(const Types::KeyframeableProperty& prop, const Types::Keyframe& kf) : Action(prop), keyframe(kf){}

            void Undo(std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>>& keyframes) const final;
        };

        struct RemoveAction : Action
        {
            Types::Keyframe keyframe;

            RemoveAction(const Types::KeyframeableProperty& prop, const Types::Keyframe& kf)
                : Action(prop), keyframe(kf){}

            void Undo(std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>>& keyframes) const final;
        };

        struct ModifyAction : Action
        {
            uint32_t id;
            ModifyAction(const Types::KeyframeableProperty& prop, uint32_t keyframeID) : Action(prop), id(keyframeID){}
        };

        struct ModifyTickAndValueAction : ModifyAction
        {
            uint32_t oldTick;
            Types::KeyframeValue oldValue;

            ModifyTickAndValueAction(const Types::KeyframeableProperty& prop, uint32_t oTick,
                                     Types::KeyframeValue oValue, uint32_t keyframeID)
                : ModifyAction(prop, keyframeID), oldTick(oTick), oldValue(oValue){}

            void Undo(std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>>& keyframes) const final;
        };

        struct ModifyTickAction : ModifyAction
        {
            uint32_t oldTick;

            ModifyTickAction(const Types::KeyframeableProperty& prop, uint32_t oTick, uint32_t keyframeID)
                : ModifyAction(prop,keyframeID), oldTick(oTick){}

            void Undo(std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>>& keyframes) const final;
        };

        struct ModifyValueAction : ModifyAction
        {
            Types::KeyframeValue oldValue;

            ModifyValueAction(const Types::KeyframeableProperty& prop, Types::KeyframeValue oValue, uint32_t keyframeID)
                : ModifyAction(prop, keyframeID), oldValue(oValue){}

            void Undo(std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>>& keyframes) const final;
        };

        struct ClearAction : Action
        {
            std::vector<Types::Keyframe> clearedKeyframes;

            ClearAction(const Types::KeyframeableProperty& prop, std::vector<Types::Keyframe> keyframes)
                : Action(prop), clearedKeyframes(keyframes){}

            void Undo(std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>>& keyframes) const final;
        };

        void Undo();

        void AddKeyframe(Types::KeyframeableProperty property, Types::Keyframe keyframeToAdd);

        void RemoveKeyframe(Types::KeyframeableProperty property, size_t indexToRemove);

        bool IsKeyframeTickBeingModified(Types::Keyframe& keyframe);

        void BeginModifyingKeyframeTick(Types::Keyframe& keyframeToModify);
        void EndModifyingKeyframeTick(Types::KeyframeableProperty property, Types::Keyframe& keyframeToModify);

        bool IsKeyframeValueBeingModified(Types::Keyframe& keyframe);

        void BeginModifyingKeyframeValue(Types::Keyframe& keyframeToModify);
        void EndModifyingKeyframeValue(Types::KeyframeableProperty property, Types::Keyframe& keyframeToModify);

        void EndModifyingKeyframeTickAndValue(Types::KeyframeableProperty property, Types::Keyframe& keyframeToModify);

        void ClearKeyframes();
        void ClearKeyframes(Types::KeyframeableProperty property);

        bool AreKeyframesBeingModified();

        Types::KeyframeValue Interpolate(const Types::KeyframeableProperty& property,
                                         const std::vector<Types::Keyframe>& keyframes, const float tick) const;
        Types::KeyframeValue Interpolate(const Types::KeyframeableProperty& property,
                                         const std::vector<Types::Keyframe>& keyframes, const uint32_t tick) const; 
        Types::KeyframeValue Interpolate(const Types::KeyframeableProperty& property, const float tick) const;
        Types::KeyframeValue Interpolate(const Types::KeyframeableProperty& property, const uint32_t tick) const;

        const Types::KeyframeableProperty& GetProperty(const Types::KeyframeablePropertyType property) const;


        void SortAndSaveKeyframes(std::vector<Types::Keyframe>& keyframes);

       private:
        KeyframeManager(){}

        Types::KeyframeValue CubicInterpolate(Types::KeyframeValueType valueType, const auto& keyframes,
                                              const float tick) const;
        Types::KeyframeValue LinearlyInterpolate(Types::KeyframeValueType valueType, const auto& keyframes,
                                                 const float tick) const;

        std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>> keyframes;
        std::unordered_map<uint32_t, uint32_t> beginningTickMap;
        std::unordered_map<uint32_t, Types::KeyframeValue> beginningValueMap;
        std::vector<std::shared_ptr<Action>> actionHistory;
    };
}  // namespace IWXMVM::Components