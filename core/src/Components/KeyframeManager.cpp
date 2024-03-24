#include "StdInclude.hpp"
#include "KeyframeManager.hpp"

#include "Resources.hpp"
#include "Utilities/MathUtils.hpp"
#include "KeyframeSerializer.hpp"
#include "Events.hpp"
#include "Mod.hpp"
#include "../Input.hpp"

namespace IWXMVM::Components
{
     Types::KeyframeableProperty campathCameraProperty(
        Types::KeyframeablePropertyType::CampathCamera, 
        ICON_FA_VIDEO " Campath Camera",
        Types::KeyframeValueType::CameraData,
        -3000, 3000 // TODO: this is not super helpful for rotation and fov
    );
    Types::KeyframeableProperty sunLightColorProperty(
        Types::KeyframeablePropertyType::SunLightColor,
        ICON_FA_SUN " Sun Light Color",
        Types::KeyframeValueType::Vector3,
        0, 255
    );
    Types::KeyframeableProperty sunLightBrightnessProperty(
        Types::KeyframeablePropertyType::SunLightBrightness,
        ICON_FA_SUN " Sun Light Brightness",
        Types::KeyframeValueType::FloatingPoint,
        0, 4
    );
    Types::KeyframeableProperty sunLightDirectionProperty(
        Types::KeyframeablePropertyType::SunLightDirection,
        ICON_FA_SUN " Sun Light Direction",
        Types::KeyframeValueType::Vector3,
        -360, 360
    );
    Types::KeyframeableProperty filmtweakBrightnessProperty(
        Types::KeyframeablePropertyType::FilmtweakBrightness,
        ICON_FA_IMAGE " Filmtweak Brightness",
        Types::KeyframeValueType::FloatingPoint,
        -1, 1
    );
    Types::KeyframeableProperty filmtweakContrastProperty(
        Types::KeyframeablePropertyType::FilmtweakContrast,
        ICON_FA_IMAGE " Filmtweak Contrast",
        Types::KeyframeValueType::FloatingPoint,
        0, 4
    );
    Types::KeyframeableProperty filmtweakDesaturationProperty(
        Types::KeyframeablePropertyType::FilmtweakDesaturation,
        ICON_FA_IMAGE " Filmtweak Desaturation",
        Types::KeyframeValueType::FloatingPoint, 
        0, 1
    );
    Types::KeyframeableProperty filmtweakTintLightProperty(
        Types::KeyframeablePropertyType::FilmtweakTintLight,
        ICON_FA_IMAGE " Filmtweak Tint Light",
        Types::KeyframeValueType::Vector3,
        0, 255
    );
    Types::KeyframeableProperty filmtweakTintDarkProperty(
        Types::KeyframeablePropertyType::FilmtweakTintDark,
        ICON_FA_IMAGE " Filmtweak Tint Dark",
        Types::KeyframeValueType::Vector3,
        0, 255
    );
    Types::KeyframeableProperty dofFarBlur(
        Types::KeyframeablePropertyType::DepthOfFieldFarBlur,
        ICON_FA_CAMERA " DOF Far Blur",
        Types::KeyframeValueType::FloatingPoint,
        0, 10
    );
    Types::KeyframeableProperty dofFarStart(
        Types::KeyframeablePropertyType::DepthOfFieldFarStart,
        ICON_FA_CAMERA " DOF Far Start",
        Types::KeyframeValueType::FloatingPoint, 
        0, 5000
    );
    Types::KeyframeableProperty dofFarEnd(
        Types::KeyframeablePropertyType::DepthOfFieldFarEnd,
        ICON_FA_CAMERA " DOF Far End", 
        Types::KeyframeValueType::FloatingPoint, 
        0, 5000
    );
    Types::KeyframeableProperty dofNearBlur(
        Types::KeyframeablePropertyType::DepthOfFieldNearBlur,
        ICON_FA_CAMERA " DOF Near Blur", 
        Types::KeyframeValueType::FloatingPoint,
        0, 10
    );
    Types::KeyframeableProperty dofNearStart(
        Types::KeyframeablePropertyType::DepthOfFieldNearStart,
        ICON_FA_CAMERA " DOF Near Start", 
        Types::KeyframeValueType::FloatingPoint, 
        0, 5000
    );
    Types::KeyframeableProperty dofNearEnd(
        Types::KeyframeablePropertyType::DepthOfFieldNearEnd,
        ICON_FA_CAMERA " DOF Near End", 
        Types::KeyframeValueType::FloatingPoint,
        0, 5000
    );
    Types::KeyframeableProperty dofBias(
        Types::KeyframeablePropertyType::DepthOfFieldBias,
        ICON_FA_CAMERA " DOF Bias", 
        Types::KeyframeValueType::FloatingPoint, 
        0.1f, 10
    );

    void KeyframeManager::HandleInput()
    {
        if (IWXMVM::Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo || CaptureManager::Get().IsCapturing())
            return;

        if (Input::KeyHeld(ImGuiKey_LeftCtrl) && Input::KeyDown(ImGuiKey_Z))
        {
            if (Input::KeyHeld(ImGuiKey_LeftShift))
            {
                Redo();
            }
            else
            {
                Undo();
            }
           
        }
    }

    void KeyframeManager::Initialize()
    {
        auto InitializeProperty = [&](auto property) { keyframes[property] = std::vector<Types::Keyframe>(); }; 
        
        InitializeProperty(campathCameraProperty);
        InitializeProperty(sunLightColorProperty);
        InitializeProperty(sunLightBrightnessProperty);
        InitializeProperty(sunLightDirectionProperty);
        InitializeProperty(filmtweakBrightnessProperty);
        InitializeProperty(filmtweakContrastProperty);
        InitializeProperty(filmtweakDesaturationProperty);
        InitializeProperty(filmtweakTintLightProperty);
        InitializeProperty(filmtweakTintDarkProperty);
        InitializeProperty(dofFarBlur);
        InitializeProperty(dofFarStart);
        InitializeProperty(dofFarEnd);
        InitializeProperty(dofNearBlur);
        InitializeProperty(dofNearStart);
        InitializeProperty(dofNearEnd);
        InitializeProperty(dofBias);

        Events::RegisterListener(EventType::PostDemoLoad, [&]() { 
            ClearKeyframes();
            actionHistory.clear();
            undidActionHistory.clear();
            Components::KeyframeSerializer::ReadRecent();
        });

        Events::RegisterListener(EventType::OnFrame, [&]() { HandleInput(); });
    }

    const Types::KeyframeableProperty& KeyframeManager::GetProperty(const Types::KeyframeablePropertyType property) const
    {
        for (auto& [p, _] : keyframes)
        {
            if (p.type == property)
                return p;
        }

        throw std::runtime_error("Unregistered keyframeable property type");
    }

    void KeyframeManager::ClearKeyframes()
    {
        for (auto& [p, k] : keyframes)
        {
            ClearKeyframes(p);
        }
    }

    void KeyframeManager::ClearKeyframes(Types::KeyframeableProperty property)
    {
        RemoveKeyframes(property, keyframes[property]);
    }

    bool KeyframeManager::AreKeyframesBeingModified()
    {
        return !beginningTickMap.empty() || !beginningValueMap.empty();
    }

    void KeyframeManager::SortAndSaveKeyframes(std::vector<Types::Keyframe>& keyframes)
    {
        std::sort(keyframes.begin(), keyframes.end(), [](const auto& a, const auto& b) { return a.tick < b.tick; });

        Components::KeyframeSerializer::WriteRecent();
    }

    void KeyframeManager::UseMostRecentAction(std::deque<std::shared_ptr<KeyframeAction>>& actions,
                                           const std::function<void(std::shared_ptr<KeyframeAction>)>& handleAction)
    {
        if (!actions.empty() && !AreKeyframesBeingModified())
        {
            std::shared_ptr<KeyframeAction> action = actions.back();
            actions.pop_back();
            if (action)
            {
                handleAction(action);
                Components::KeyframeManager::Get().SortAndSaveKeyframes(GetKeyframes(action->property));
            }
        }
    }

    void KeyframeManager::Undo()
    {
        UseMostRecentAction(actionHistory, [&](std::shared_ptr<KeyframeAction> action)
            {
                action->GetUndoAction()->DoAction();
                AddAction_Internal(undidActionHistory, action);
                nextActionWipeUndidHistory = true;
            });
    }

    void KeyframeManager::Redo()
    {
        UseMostRecentAction(undidActionHistory, [&](std::shared_ptr<KeyframeAction> action) 
            {
                action->DoAction();
                AddAction_Internal(actionHistory, action);
            });
    }

    void KeyframeManager::AddKeyframe(Types::KeyframeableProperty property, Types::Keyframe keyframeToAdd)
    {
        AddKeyframes(property, std::vector<Types::Keyframe>{keyframeToAdd});
    }

    void KeyframeManager::AddKeyframes(Types::KeyframeableProperty property,
                                       const std::vector<Types::Keyframe> keyframesToAdd)
    {
        std::shared_ptr<AddKeyframesAction> addAction = std::make_shared<AddKeyframesAction>(property, keyframesToAdd);
        addAction->DoAction();
        AddActionToHistory(addAction);
    }

    void KeyframeManager::RemoveKeyframe(Types::KeyframeableProperty property,
                                         std::vector<Types::Keyframe>::iterator it)
    {
        RemoveKeyframes(property, {*it});
    }

    void KeyframeManager::RemoveKeyframe(Types::KeyframeableProperty property, size_t indexToRemove)
    {
        RemoveKeyframes(property, {keyframes[property].at(indexToRemove)});
    }

    void KeyframeManager::RemoveKeyframes(Types::KeyframeableProperty property,
                                          std::vector<Types::Keyframe> keyframesToRemove)
    {
        if (!keyframes[property].empty())
        {
            std::shared_ptr<RemoveKeyframesAction> removeAction =
                std::make_shared<RemoveKeyframesAction>(property, keyframesToRemove);
            removeAction->DoAction();
            AddActionToHistory(removeAction);
        }
    }

    bool KeyframeManager::IsKeyframeTickBeingModified(Types::Keyframe& keyframe)
    {
        return beginningTickMap.find(keyframe.id) != beginningTickMap.end();
    }

    void KeyframeManager::BeginModifyingKeyframeTick(Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("Begin Modifying Tick " + std::to_string(keyframeToModify.id));
        beginningTickMap[keyframeToModify.id] = keyframeToModify.tick;
    }

    void KeyframeManager::EndModifyingKeyframeTick(Types::KeyframeableProperty property,
                                                   Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("End Modifying Tick " + std::to_string(keyframeToModify.id));
        std::shared_ptr<ModifyTickAction> modifyAction = std::make_shared<ModifyTickAction>(
            property, beginningTickMap[keyframeToModify.id], keyframeToModify.tick, keyframeToModify.id);
        AddActionToHistory(modifyAction);
        beginningTickMap.erase(keyframeToModify.id);
    }

    bool KeyframeManager::IsKeyframeValueBeingModified(Types::Keyframe& keyframe)
    {
        return beginningValueMap.find(keyframe.id) != beginningValueMap.end();
    }

    void KeyframeManager::BeginModifyingKeyframeValue(Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("Begin Modifying Value " + std::to_string(keyframeToModify.id));
        beginningValueMap[keyframeToModify.id] = keyframeToModify.value;
    }

    void KeyframeManager::EndModifyingKeyframeValue(Types::KeyframeableProperty property,
                                                   Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("End Modifying Value " + std::to_string(keyframeToModify.id));
        std::shared_ptr<ModifyValueAction> modifyAction = std::make_shared<ModifyValueAction>(
            property, beginningValueMap[keyframeToModify.id], keyframeToModify.value, keyframeToModify.id);
        AddActionToHistory(modifyAction);
        beginningValueMap.erase(keyframeToModify.id);
    }

    void KeyframeManager::EndModifyingKeyframeTickAndValue(Types::KeyframeableProperty property,
                                                    Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("End Modifying Tick & Value " + std::to_string(keyframeToModify.id));
        
        std::shared_ptr<ModifyTickAndValueAction> modifyAction = std::make_shared<ModifyTickAndValueAction>(
            property, beginningTickMap[keyframeToModify.id], keyframeToModify.tick,
                                                       beginningValueMap[keyframeToModify.id],keyframeToModify.value, keyframeToModify.id);
        AddActionToHistory(modifyAction);
        beginningTickMap.erase(keyframeToModify.id);
        beginningValueMap.erase(keyframeToModify.id);
    }

    Types::KeyframeValue KeyframeManager::Interpolate(const Types::KeyframeableProperty& property,
                                                      const std::vector<Types::Keyframe>& keyframes,
                                                      const float tick) const
    {
        if (keyframes.empty())
            return Types::KeyframeValue::GetDefaultValue(property.valueType);

        if (tick < keyframes.front().tick)
            return keyframes.front().value;
        if (tick > keyframes.back().tick)
            return keyframes.back().value;

        // TODO: interpolation selection in the future
        if (keyframes.size() < 4)
            return LinearlyInterpolate(property.valueType, keyframes, tick);

        return CubicInterpolate(property.valueType, keyframes, tick);
    }

    Types::KeyframeValue KeyframeManager::Interpolate(const Types::KeyframeableProperty& property,
                                                      const std::vector<Types::Keyframe>& keyframes,
                                                      const uint32_t tick) const 
    {
        return Interpolate(property, keyframes, static_cast<float>(tick));
    }

    Types::KeyframeValue KeyframeManager::Interpolate(const Types::KeyframeableProperty& property, const float tick) const
    {
        const auto& keyframes = KeyframeManager::Get().GetKeyframes(property);
        return Interpolate(property, keyframes, tick);
    }

    Types::KeyframeValue KeyframeManager::Interpolate(const Types::KeyframeableProperty& property, const uint32_t tick) const 
    {
        return Interpolate(property, static_cast<float>(tick));
    }

    Types::KeyframeValue KeyframeManager::CubicInterpolate(Types::KeyframeValueType valueType,
                                                           const auto& keyframes,
                                                           const float tick) const
    {
        switch (valueType)
        {
            case Types::KeyframeValueType::FloatingPoint:
                return MathUtils::InterpolateCubicSpline(keyframes, 0, tick);
            case Types::KeyframeValueType::Vector3:
                return glm::vec3(
                    MathUtils::InterpolateCubicSpline(keyframes, 0, tick),
                    MathUtils::InterpolateCubicSpline(keyframes, 1, tick),
                    MathUtils::InterpolateCubicSpline(keyframes, 2, tick)
                );
            case Types::KeyframeValueType::CameraData:
            {
                return Types::KeyframeValue(
                    Types::CameraData(
                        glm::vec3(
                            MathUtils::InterpolateCubicSpline(keyframes, 0, tick),
                            MathUtils::InterpolateCubicSpline(keyframes, 1, tick),
                            MathUtils::InterpolateCubicSpline(keyframes, 2, tick)
                        ),
                        // TODO: do proper quaternion interpolation here
                        glm::vec3(
                            MathUtils::InterpolateCubicSpline(keyframes, 3, tick),
                            MathUtils::InterpolateCubicSpline(keyframes, 4, tick),
                            MathUtils::InterpolateCubicSpline(keyframes, 5, tick)
                        ), 
                        MathUtils::InterpolateCubicSpline(keyframes, 6, tick)
                    )
                );
            }
            default:
                return Types::KeyframeValue(0.0f);
        }
    }

    Types::KeyframeValue KeyframeManager::LinearlyInterpolate(Types::KeyframeValueType valueType, const auto& keyframes,
                                                              const float tick) const
    {
        if (keyframes.size() == 1)
            return keyframes.front().value;

        std::int32_t p0Idx = 0, p1Idx = 1;
        for (std::size_t i = 0; i < keyframes.size() - 1; i++)
        {
            if (tick >= keyframes[i].tick && tick <= keyframes[i + 1].tick)
            {
                p0Idx = i;
                p1Idx = i + 1;
                break;
            }
        }

        assert(p0Idx != -1 || p1Idx != -1);
        assert(keyframes[p0Idx].tick < keyframes[p1Idx].tick);
        assert(p0Idx != p1Idx);

        const auto& p0 = keyframes[p0Idx];
        const auto& p1 = keyframes[p1Idx];

        const float t = static_cast<float>(tick - p0.tick) / static_cast<float>(p1.tick - p0.tick);
        switch (valueType)
        {
            case Types::KeyframeValueType::FloatingPoint:
                return Types::KeyframeValue((1.0f - t) * p0.value.floatingPoint + t * p1.value.floatingPoint);
            case Types::KeyframeValueType::Vector3:
                return Types::KeyframeValue((1.0f - t) * p0.value.vector3 + t * p1.value.vector3);
            case Types::KeyframeValueType::CameraData:
                return Types::KeyframeValue(
                    Types::CameraData((1.0f - t) * p0.value.cameraData.position + t * p1.value.cameraData.position,
                                      (1.0f - t) * p0.value.cameraData.rotation + t * p1.value.cameraData.rotation,
                                      (1.0f - t) * p0.value.cameraData.fov + t * p1.value.cameraData.fov));
            default:
                return Types::KeyframeValue(0.0f);
        }
    }


    void KeyframeManager::AddAction_Internal(std::deque<std::shared_ptr<KeyframeAction>>& actionQue, std::shared_ptr<KeyframeAction> action) const
    {
        while (actionQue.size() >= MAX_ACTIONHISTORY)
            actionQue.pop_front();
        actionQue.push_back(action);
    }

    void KeyframeManager::AddActionToHistory(std::shared_ptr<KeyframeAction> action)
    {
        if (nextActionWipeUndidHistory)
        {
            undidActionHistory.clear();
            nextActionWipeUndidHistory = false;
        }
        AddAction_Internal(actionHistory , action);
    }

    void KeyframeManager::ModifyTickAction::DoAction() const
    {
        if (auto it = GetKeyframe(id); it != GetKeyframes().end())
            it->tick = newTick;
    }

    std::unique_ptr<KeyframeManager::KeyframeAction> KeyframeManager::ModifyTickAction::GetUndoAction() const
    {
        return std::make_unique<ModifyTickAction>(property,newTick,oldTick,id);
    }

    std::vector<Types::Keyframe>::iterator KeyframeManager::KeyframeAction::GetKeyframe(uint32_t id) const
    {
        auto& keyframes = GetKeyframes();
        auto it = std::find_if(keyframes.begin(), keyframes.end(), [&](const Types::Keyframe& kf) { return kf.id == id; });
        return it;
    }

    std::vector<Types::Keyframe>& KeyframeManager::KeyframeAction::GetKeyframes() const
    {
        return KeyframeManager::Get().GetKeyframes(property);
    }

    void KeyframeManager::ModifyValueAction::DoAction() const
    {
        if (auto it = GetKeyframe(id); it != GetKeyframes().end())
            it->value = newValue;
    }

    std::unique_ptr<KeyframeManager::KeyframeAction> KeyframeManager::ModifyValueAction::GetUndoAction() const
    {
        return std::make_unique<ModifyValueAction>(property, newValue, oldValue, id);
    }

    void KeyframeManager::ModifyTickAndValueAction::DoAction() const
    {
        if (auto it = GetKeyframe(id); it != GetKeyframes().end())
        {
            it->tick = newTick;
            it->value = newValue;
        }
    }

    std::unique_ptr<KeyframeManager::KeyframeAction> KeyframeManager::ModifyTickAndValueAction::GetUndoAction() const
    {
        return std::make_unique<ModifyTickAndValueAction>(property,newTick,oldTick,newValue,oldValue,id);
    }

    void KeyframeManager::RemoveKeyframesAction::DoAction() const
    {
        for (auto& keyframe : keyframes)
        {
            if (auto it = GetKeyframe(keyframe.id); it != GetKeyframes().end())
            {
                GetKeyframes().erase(it);
            }
        }
    }

    std::unique_ptr<KeyframeManager::KeyframeAction> KeyframeManager::RemoveKeyframesAction::GetUndoAction() const
    {
        return std::make_unique<AddKeyframesAction>(property,keyframes);
    }

    void KeyframeManager::AddKeyframesAction::DoAction() const
    {
        for (auto& keyframe : keyframes)
        {
            GetKeyframes().emplace_back(keyframe);
        }
    }

    std::unique_ptr<KeyframeManager::KeyframeAction> KeyframeManager::AddKeyframesAction::GetUndoAction() const
    {
        return std::make_unique<RemoveKeyframesAction>(property, keyframes);
    }

}  // namespace IWXMVM::Components
