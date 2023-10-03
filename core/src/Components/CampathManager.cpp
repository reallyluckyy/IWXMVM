#include "StdInclude.hpp"
#include "CampathManager.hpp"

#include "Mod.hpp"
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

        // Modifying markers shouldn't be possible while in Dolly mode
        if (activeCamera->GetMode() != Camera::Mode::Dolly)
        {
            if (Input::BindDown(InputConfiguration::BIND_DOLLY_ADD_MARKER))
            {
                Types::Marker marker{};
                marker.position = activeCamera->GetPosition();
                marker.rotation = activeCamera->GetRotation();
                marker.fov = activeCamera->GetFov();
                marker.tick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
                AddMarker(marker);

                LOG_DEBUG("Placed marker at (x: {}; y: {}; z: {}) with (pitch: {}; yaw: {}; roll: {}) at tick {}",
                          marker.position.x, marker.position.y, marker.position.z, marker.rotation.x, marker.rotation.y,
                          marker.rotation.z, marker.tick);
            }

            if (Input::BindDown(InputConfiguration::BIND_DOLLY_CLEAR_MARKERS))
            {
                markers.clear();

                LOG_DEBUG("Markers cleared");
            }

            if (Input::BindDown(InputConfiguration::BIND_DOLLY_PLAY_PATH))
            {
                CameraManager::Get().SetActiveCamera(Camera::Mode::Dolly);
            }
        }
    }

    void CampathManager::AddMarker(Types::Marker marker)
    {
        // If there's another marker at this tick already, overwrite it
        for (auto& m : markers)
        {
            if (m.tick == marker.tick)
            {
                m = marker;
                return;
            }
        }

        markers.push_back(marker);

        // Vector needs to always be sorted in ascending order by marker ticks
        std::sort(markers.begin(), markers.end(),
                  [](const Types::Marker& a, const Types::Marker& b) { return a.tick < b.tick; });
    }

    void CampathManager::Initialize()
    {
        Events::RegisterListener(EventType::OnFrame, [&]() { Update(); });
    }
}  // namespace IWXMVM::Components
