#include "StdInclude.hpp"
#include "CampathManager.hpp"

#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "../Events.hpp"
#include "../Input.hpp"
#include "Playback.hpp"

namespace IWXMVM::Components
{
    void CampathManager::Update()
    {
        if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
        {
            return;
        }

        auto& activeCamera = CameraManager::Get().GetActiveCamera();

        // TODO: There should be better feedback for making it clear to the user that they can place nodes (from their
        // current camera position) while in modes that are not the usual freecam. Its a bit unintuitive being
        // able to place nodes while in POV for example.
        if (activeCamera->GetMode() != Camera::Mode::Dolly)
        {
            const auto& property = KeyframeManager::Get().GetProperty(Types::KeyframeablePropertyType::CampathCamera);

            if (Input::BindDown(Action::DollyAddNode))
            {
                const auto tick = Components::Playback::GetTimelineTick();
                
                for (const auto& keyframe : KeyframeManager::Get().GetKeyframes(property))
                {
                    if (keyframe.tick == tick)
                    {
                        LOG_WARN("A campath node already exists at the current tick. Not creating a new one.");
                        return;
                    }
                }

                Types::CameraData node;
                node.position = activeCamera->GetPosition();
                node.rotation = activeCamera->GetRotation();
                node.fov = activeCamera->GetFov();

                KeyframeManager::Get().AddKeyframe(property, Types::Keyframe(property, tick, node));

                LOG_DEBUG("Placed node at (x: {}; y: {}; z: {}) with (pitch: {}; yaw: {}; roll: {}) at tick {}",
                          node.position.x, node.position.y, node.position.z, node.rotation.x, node.rotation.y,
                          node.rotation.z, tick);

                KeyframeManager::Get().SortAndSaveKeyframes(KeyframeManager::Get().GetKeyframes(property));
            }

            if (Input::BindDown(Action::DollyClearNodes))
            {
                KeyframeManager::Get().ClearKeyframes(property);

                LOG_DEBUG("Nodes cleared");
            }

            if (Input::BindDown(Action::DollyPlayPath))
            {
                CameraManager::Get().SetActiveCamera(Camera::Mode::Dolly);
                if (!KeyframeManager::Get().GetKeyframes(property).empty())
                {
                    Playback::SetTickDelta(KeyframeManager::Get().GetKeyframes(property).front().tick -
                                               Components::Playback::GetTimelineTick(), true);
                }
            }

            if (Input::BindDown(Action::FirstPersonToggle))
            {
                if (activeCamera->GetMode() == Camera::Mode::FirstPerson)
                {
                    auto prevCamMode = CameraManager::Get().GetPreviousActiveCamera()->GetMode();
                    CameraManager::Get().SetActiveCamera(prevCamMode);
                    if (CameraManager::Get().GetActiveCamera()->GetMode() == Camera::Mode::Free)
                    {
                        UI::UIManager::Get().GetUIComponent(UI::Component::GameView)->SetHasFocus(true);
                    }
                }
                else
                {
                    CameraManager::Get().SetActiveCamera(Camera::Mode::FirstPerson);
                }
            } 
        }
        else
        {
            if (Input::BindDown(Action::DollyPlayPath))
            {
                CameraManager::Get().SetActiveCamera(CameraManager::Get().GetPreviousActiveCamera()->GetMode());
                if (CameraManager::Get().GetActiveCamera()->GetMode() == Camera::Mode::Free)
                {
                    UI::UIManager::Get().GetUIComponent(UI::Component::GameView)->SetHasFocus(true);
                }  
            }
        }
    }

    void CampathManager::Initialize()
    {
        Events::RegisterListener(EventType::OnFrame, [&]() { Update(); });
    }
}  // namespace IWXMVM::Components
