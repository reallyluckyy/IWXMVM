#include "StdInclude.hpp"
#include "CampathManager.hpp"

#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "../Events.hpp"
#include "../Input.hpp"

namespace IWXMVM::Components
{
    std::string_view CampathManager::GetInterpolationModeLabel(InterpolationMode interpolationMode)
    {
        switch (interpolationMode)
        {
            case InterpolationMode::Linear:
                return "Linear";
            case InterpolationMode::CatmullRom:
                return "Catmull Rom";
            default:
                return "Unknown";
        }
    }

    std::vector<InterpolationMode> CampathManager::GetInterpolationModes()
    {
        std::vector<InterpolationMode> interpolationModes;
        for (int i = 0; i < (int)InterpolationMode::Count; i++)
        {
            interpolationModes.push_back((InterpolationMode)i);
        }
        return interpolationModes;
    }

    Types::CampathNode LinearlyInterpolate(const std::vector<Types::CampathNode>& campathNodes, uint32_t tick)
    {
        // We want to know the campathNodes that the current tick is sandwiched between
        // p0 will be the node before the current tick, p1 the node after the current tick

        std::size_t p0Idx = 0, p1Idx = 1;
        for (std::size_t i = 0; i < campathNodes.size() - 1; i++)
        {
            if (tick >= campathNodes[i].tick && tick <= campathNodes[i + 1].tick)
            {
                p0Idx = i;
                p1Idx = i + 1;
                break;
            }
        }

        const auto& p0 = campathNodes[p0Idx];
        const auto& p1 = campathNodes[p1Idx];

        // Interpolate between p0 and p1
        const float t = static_cast<float>(tick - p0.tick) / static_cast<float>(p1.tick - p0.tick);
        return Types::CampathNode((1.0f - t) * p0.position + t * p1.position,
                                  (1.0f - t) * p0.rotation + t * p1.rotation, (1.0f - t) * p0.fov + t * p1.fov, tick);
    }

    Types::CampathNode CatmullRomInterpolate(const std::vector<Types::CampathNode>& campathNodes, uint32_t tick)
    {
        std::int32_t p1Idx = 0, p2Idx = 1;
        for (std::size_t i = 0; i < campathNodes.size() - 1; i++)
        {
            if (tick >= campathNodes[i].tick && tick <= campathNodes[i + 1].tick)
            {
                p1Idx = i;
                p2Idx = i + 1;
                break;
            }
        }

        const auto& p0 = campathNodes[p1Idx - 1 < 0 ? 0 : p1Idx - 1];
        const auto& p1 = campathNodes[p1Idx];
        const auto& p2 = campathNodes[p2Idx];
        const auto& p3 = campathNodes[p2Idx + 1 >= campathNodes.size() ? campathNodes.size() - 1 : p2Idx + 1];

        float t = (float)(tick - p1.tick) / (float)(p2.tick - p1.tick);

        return Types::CampathNode(
            glm::catmullRom(p0.position, p1.position, p2.position, p3.position, t),
            glm::catmullRom(p0.rotation, p1.rotation, p2.rotation, p3.rotation, t),
            glm::catmullRom(glm::vec1(p0.fov), glm::vec1(p1.fov), glm::vec1(p2.fov), glm::vec1(p3.fov), t).x, tick);
    }

    Types::CampathNode CampathManager::Interpolate(uint32_t tick)
    {
        switch (interpolationMode)
        {
            case InterpolationMode::Linear:
                return LinearlyInterpolate(nodes, tick);
            case InterpolationMode::CatmullRom:
                return CatmullRomInterpolate(nodes, tick);
            default:
                return Types::CampathNode();
        }
    }

    int32_t CampathManager::GetRequiredNodeCount() const
    {
        switch (interpolationMode)
        {
            case InterpolationMode::Linear:
                return 2;
            case InterpolationMode::CatmullRom:
                return 2;
            default:
                return 0;
        }
    }

    void CampathManager::Update()
    {
        if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
        {
            return;
        }

        auto& activeCamera = CameraManager::Get().GetActiveCamera();

        // TODO: There should be better feedback for making it clear to the user that they can place nodes (from their
        // current camera position) TOOD: while in modes that are not the usual freecam. Its a bit unintuitive being
        // able to place nodes while in POV for example.
        if (UI::UIManager::Get().GetSelectedTab() == UI::Tab::Camera && activeCamera->GetMode() != Camera::Mode::Dolly)
        {
            if (Input::BindDown(Bind::DollyAddNode))
            {
                Types::CampathNode node{};
                node.position = activeCamera->GetPosition();
                node.rotation = activeCamera->GetRotation();
                node.fov = activeCamera->GetFov();
                node.tick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
                AddNode(node);

                LOG_DEBUG("Placed node at (x: {}; y: {}; z: {}) with (pitch: {}; yaw: {}; roll: {}) at tick {}",
                          node.position.x, node.position.y, node.position.z, node.rotation.x, node.rotation.y,
                          node.rotation.z, node.tick);
            }

            if (Input::BindDown(Bind::DollyClearNodes))
            {
                nodes.clear();

                LOG_DEBUG("Nodes cleared");
            }

            if (Input::BindDown(Bind::DollyPlayPath))
            {
                CameraManager::Get().SetActiveCamera(Camera::Mode::Dolly);
            }
        }
    }

    void CampathManager::AddNode(Types::CampathNode node)
    {
        // If there's another node at this tick already, overwrite it
        // TODO: Do we want this behaviour? I feel like there should be a warning prompt here
        for (auto& m : nodes)
        {
            if (m.tick == node.tick)
            {
                m = node;
                return;
            }
        }

        nodes.push_back(node);

        // Vector needs to always be sorted in ascending order by node ticks
        std::sort(nodes.begin(), nodes.end(),
                  [](const Types::CampathNode& a, const Types::CampathNode& b) { return a.tick < b.tick; });
    }

    void CampathManager::Initialize()
    {
        Events::RegisterListener(EventType::OnFrame, [&]() { Update(); });
    }
}  // namespace IWXMVM::Components
