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
            Undo();
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
            k.clear();
        }
    }

    void KeyframeManager::SortAndSaveKeyframes(std::vector<Types::Keyframe>& keyframes)
    {
        std::sort(keyframes.begin(), keyframes.end(), [](const auto& a, const auto& b) { return a.tick < b.tick; });

        Components::KeyframeSerializer::WriteRecent();
    }

    void KeyframeManager::Undo()
    {
        if (!actionHistory.empty())
        {
            Action* action = actionHistory.back();
            if (AddAction* addAction = dynamic_cast<AddAction*>(action); addAction)
            {
                for (size_t i = 0; i < keyframes[addAction->property].size(); i++)
                {
                    if (keyframes[addAction->property][i].tick == addAction->keyframe.tick)
                    {
                        keyframes[addAction->property].erase(keyframes[addAction->property].begin() + i);
                        break;
                    }
                }
            }
            else if (RemoveAction* removeAction = dynamic_cast<RemoveAction*>(action); removeAction)
            {
                keyframes[removeAction->property].push_back(removeAction->keyframe);
            }
            else if (ModifyTickAndValueAction* modifyTickAndValueAction =
                         dynamic_cast<ModifyTickAndValueAction*>(action);
                     modifyTickAndValueAction)
            {
                for (size_t i = 0; i < keyframes[modifyTickAndValueAction->property].size(); i++)
                {
                    if (keyframes[modifyTickAndValueAction->property][i].tick == modifyTickAndValueAction->newTick)
                    {
                        keyframes[modifyTickAndValueAction->property][i].tick = modifyTickAndValueAction->oldTick;
                        keyframes[modifyTickAndValueAction->property][i].value = modifyTickAndValueAction->oldValue;
                        break;
                    }
                }
            }
            else if (ModifyTickAction* modifyTickAction = dynamic_cast<ModifyTickAction*>(action); modifyTickAction)
            {
                for (size_t i = 0; i < keyframes[modifyTickAction->property].size(); i++)
                {
                    if (keyframes[modifyTickAction->property][i].tick == modifyTickAction->newTick)
                    {
                        keyframes[modifyTickAction->property][i].tick = modifyTickAction->oldTick;
                        break;
                    }
                }
            }
            else if (ModifyValueAction* modifyValueAction = dynamic_cast<ModifyValueAction*>(action); modifyValueAction)
            {
                for (size_t i = 0; i < keyframes[modifyValueAction->property].size(); i++)
                {
                    if (keyframes[modifyValueAction->property][i].tick == modifyValueAction->tick)
                    {
                        keyframes[modifyValueAction->property][i].value = modifyValueAction->oldValue;
                        break;
                    }
                }
            }
            Components::KeyframeManager::Get().SortAndSaveKeyframes(GetKeyframes(action->property));
            actionHistory.pop_back();
            delete action; 
        }
    }

    void KeyframeManager::AddKeyframe(Types::KeyframeableProperty property, Types::Keyframe keyframeToAdd)
    {
        keyframes[property].emplace_back(keyframeToAdd);
        AddAction* addAction = new AddAction(property, keyframeToAdd);
        actionHistory.push_back(addAction);
    }

    void KeyframeManager::RemoveKeyframe(Types::KeyframeableProperty property, size_t indexToRemove)
    {
        RemoveAction* removeAction = new RemoveAction(property, keyframes[property].at(indexToRemove));
        actionHistory.push_back(removeAction);
        keyframes[property].erase(keyframes[property].begin() + indexToRemove);
    }


    void KeyframeManager::RemoveKeyframe(Types::KeyframeableProperty property, Types::Keyframe keyframeToRemove)
    {
        for (size_t i = 0; i < keyframes[property].size(); i++)
        {
            if (keyframes[property][i].tick == keyframeToRemove.tick)
            {
                RemoveKeyframe(property,i);
                return;
            }
        }
    }

    bool KeyframeManager::IsKeyframeTickBeingModified(Types::Keyframe& keyframe)
    {
        return beginningTickMap.find(keyframe.id) != beginningTickMap.end();
    }

    void KeyframeManager::BeginModifyingKeyframeTick(Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("Begin Tick");
        beginningTickMap[keyframeToModify.id] = keyframeToModify.tick;
    }

    void KeyframeManager::EndModifyingKeyframeTick(Types::KeyframeableProperty property,
                                                   Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("End Tick");
        ModifyTickAction* modifyAction =
            new ModifyTickAction(property, beginningTickMap[keyframeToModify.id], keyframeToModify.tick);
        actionHistory.push_back(modifyAction);
        beginningTickMap.erase(keyframeToModify.id);
    }

    void KeyframeManager::ModifyKeyframeTick(Types::KeyframeableProperty property, Types::Keyframe& keyframeToModify,
                                         uint32_t newTick)
    {
        if (beginningTickMap.find(keyframeToModify.id) == beginningTickMap.end())
        {
            ModifyTickAction* modifyAction = new ModifyTickAction(property, keyframeToModify.tick, newTick);
            actionHistory.push_back(modifyAction);
        }
        keyframeToModify.tick = newTick;
    }

    bool KeyframeManager::IsKeyframeValueBeingModified(Types::Keyframe& keyframe)
    {
        return beginningValueMap.find(keyframe.id) != beginningValueMap.end();
    }

    void KeyframeManager::BeginModifyingKeyframeValue(Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("Begin Value");
        beginningValueMap[keyframeToModify.id] = keyframeToModify.value;
    }

    void KeyframeManager::EndModifyingKeyframeValue(Types::KeyframeableProperty property,
                                                   Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("End Value");
        ModifyValueAction* modifyAction =
            new ModifyValueAction(property, beginningValueMap[keyframeToModify.id], keyframeToModify.tick);
        actionHistory.push_back(modifyAction);
        beginningValueMap.erase(keyframeToModify.id);
    }

    void KeyframeManager::ModifyKeyframeValue(Types::KeyframeableProperty property, Types::Keyframe& keyframeToModify,
                                              Types::KeyframeValue newValue)
    {
        if (beginningValueMap.find(keyframeToModify.id) == beginningValueMap.end())
        {
            ModifyValueAction* modifyAction =
                new ModifyValueAction(property, keyframeToModify.value, keyframeToModify.tick);
            actionHistory.push_back(modifyAction);
        }
        keyframeToModify.value = newValue;
    }

    bool KeyframeManager::IsKeyframeTickAndValueBeingModified(Types::Keyframe& keyframe)
    {
        return beginningValueMap.find(keyframe.id) != beginningValueMap.end() &&
               beginningTickMap.find(keyframe.id) != beginningTickMap.end();
    }

    void KeyframeManager::BeginModifyingKeyframeTickAndValue(Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("Begin BOTH");
        beginningTickMap[keyframeToModify.id] = keyframeToModify.tick;
        beginningValueMap[keyframeToModify.id] = keyframeToModify.value;
    }

    void KeyframeManager::EndModifyingKeyframeTickAndValue(Types::KeyframeableProperty property,
                                                    Types::Keyframe& keyframeToModify)
    {
        LOG_DEBUG("End BOTH");
        ModifyTickAndValueAction* modifyAction = new ModifyTickAndValueAction(property, beginningTickMap[keyframeToModify.id], keyframeToModify.tick,
                                         beginningValueMap[keyframeToModify.id]);
        actionHistory.push_back(modifyAction);
        beginningTickMap.erase(keyframeToModify.id);
        beginningValueMap.erase(keyframeToModify.id);
    }

    void KeyframeManager::ModifyKeyframeTickAndValue(Types::KeyframeableProperty property,
                                                     Types::Keyframe& keyframeToModify, uint32_t newTick,
                                                     Types::KeyframeValue newValue)
    {
        if (beginningValueMap.find(keyframeToModify.id) == beginningValueMap.end() &&
            beginningTickMap.find(keyframeToModify.id) == beginningTickMap.end())
        {
            ModifyTickAndValueAction* modifyAction =
                new ModifyTickAndValueAction(property, keyframeToModify.tick, newTick, keyframeToModify.value);
            actionHistory.push_back(modifyAction);
        }
        keyframeToModify.tick = newTick;
        keyframeToModify.value = newValue;
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


    KeyframeManager::~KeyframeManager()
    {
        for (auto& action : actionHistory)
        {
            delete action;
        }
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

}  // namespace IWXMVM::Components
