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

            virtual void DoAction() const = 0;
            virtual std::unique_ptr<KeyframeManager::Action> GetUndoAction() const = 0;

           protected:
            std::vector<Types::Keyframe>& GetKeyframes() const;
            std::vector<Types::Keyframe>::iterator GetKeyframe(uint32_t id) const;
        };

        struct AddAction : KeyframeManager::Action
        {
            Types::Keyframe keyframe;

            AddAction(const Types::KeyframeableProperty& prop, const Types::Keyframe& kf)
                : KeyframeManager::Action(prop), keyframe(kf){}

            void DoAction() const final;
            std::unique_ptr<KeyframeManager::Action> GetUndoAction() const final;
        };

        struct RemoveAction : KeyframeManager::Action
        {
            Types::Keyframe keyframe;

            RemoveAction(const Types::KeyframeableProperty& prop, const Types::Keyframe& kf)
                : KeyframeManager::Action(prop), keyframe(kf){}

            void DoAction() const final;
            std::unique_ptr<KeyframeManager::Action> GetUndoAction() const final;
        };

        struct ModifyAction : KeyframeManager::Action
        {
            uint32_t id;
            ModifyAction(const Types::KeyframeableProperty& prop, uint32_t keyframeID) : Action(prop), id(keyframeID){}
        };

        struct ModifyTickAndValueAction : ModifyAction
        {
            uint32_t oldTick;
            uint32_t newTick;
            Types::KeyframeValue oldValue;
            Types::KeyframeValue newValue;

            ModifyTickAndValueAction(const Types::KeyframeableProperty& prop, uint32_t oTick, uint32_t nTick,
                                     Types::KeyframeValue oValue, Types::KeyframeValue nValue, uint32_t keyframeID)
                : ModifyAction(prop, keyframeID), oldTick(oTick), newTick(nTick), oldValue(oValue), newValue(nValue){}

            void DoAction() const final;
            std::unique_ptr<KeyframeManager::Action> GetUndoAction() const final;
        };

        struct ModifyTickAction : ModifyAction
        {
            uint32_t oldTick;
            uint32_t newTick;

            ModifyTickAction(const Types::KeyframeableProperty& prop, uint32_t oTick, uint32_t nTick, uint32_t keyframeID)
                : ModifyAction(prop,keyframeID), oldTick(oTick), newTick(nTick){}

            void DoAction() const final;
            std::unique_ptr<KeyframeManager::Action> GetUndoAction() const final;
        };

        struct ModifyValueAction : ModifyAction
        {
            Types::KeyframeValue oldValue;
            Types::KeyframeValue newValue;

            ModifyValueAction(const Types::KeyframeableProperty& prop, Types::KeyframeValue oValue,
                              Types::KeyframeValue nValue, uint32_t keyframeID)
                : ModifyAction(prop, keyframeID), oldValue(oValue), newValue(nValue){}

            void DoAction() const final;
            std::unique_ptr<KeyframeManager::Action> GetUndoAction() const final;
        };
        
        struct ManyKeyframesAction : Action
        {
            std::vector<Types::Keyframe> keyframes;
            ManyKeyframesAction(const Types::KeyframeableProperty& prop, std::vector<Types::Keyframe> keyframes)
                : Action(prop), keyframes(keyframes){}
        };

        struct RemoveManyKeyframes : ManyKeyframesAction
        {
            RemoveManyKeyframes(const Types::KeyframeableProperty& prop, std::vector<Types::Keyframe> keyframes)
                : ManyKeyframesAction(prop,keyframes){}

            virtual void DoAction() const final;
            std::unique_ptr<KeyframeManager::Action> GetUndoAction() const final;
        };

        struct AddManyKeyframes : ManyKeyframesAction
        {
            AddManyKeyframes(const Types::KeyframeableProperty& prop, std::vector<Types::Keyframe> keyframes)
                : ManyKeyframesAction(prop, keyframes){}

            virtual void DoAction() const final;
            std::unique_ptr<KeyframeManager::Action> GetUndoAction() const final;
        };

        void Undo();
        void Redo();

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

        void AddActionToHistory(std::shared_ptr<Action> action);
        

        void AddActionToUndidHistory(std::shared_ptr<Action> action);

        std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>> keyframes;
        std::unordered_map<uint32_t, uint32_t> beginningTickMap;
        std::unordered_map<uint32_t, Types::KeyframeValue> beginningValueMap;
        const size_t MAX_ACTIONHISTORY = 25;
        bool nextActionWipes_undidActionHistory;
        std::deque<std::shared_ptr<Action>> undidActionHistory;
        std::deque<std::shared_ptr<Action>> actionHistory;
    };
}  // namespace IWXMVM::Components