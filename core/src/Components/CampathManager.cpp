#include "StdInclude.hpp"
#include "CampathManager.hpp"

#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "../Events.hpp"
#include "../Input.hpp"

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
                const auto tick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
                
                for (const auto& keyframe : KeyframeManager::Get().GetKeyframes(property))
                {
                    if (keyframe.tick == tick)
                        return;
                }

                Types::CameraData node;
                node.position = activeCamera->GetPosition();
                node.rotation = activeCamera->GetRotation();
                node.fov = activeCamera->GetFov();

                auto& keyframes = KeyframeManager::Get().GetKeyframes(property);
                keyframes.push_back(
                    Types::Keyframe(property, tick, node));

                LOG_DEBUG("Placed node at (x: {}; y: {}; z: {}) with (pitch: {}; yaw: {}; roll: {}) at tick {}",
                          node.position.x, node.position.y, node.position.z, node.rotation.x, node.rotation.y,
                          node.rotation.z, tick);

                KeyframeManager::Get().SortAndSaveKeyframes(keyframes);
            }

            if (Input::BindDown(Action::DollyClearNodes))
            {
                KeyframeManager::Get().GetKeyframes(property).clear();

                LOG_DEBUG("Nodes cleared");
            }

            if (Input::BindDown(Action::DollyPlayPath))
            {
                CameraManager::Get().SetActiveCamera(Camera::Mode::Dolly);
            }
        }
    }

    void CampathManager::Initialize()
    {
        Events::RegisterListener(EventType::OnFrame, [&]() { Update(); });
    }
}  // namespace IWXMVM::Components
