#include "StdInclude.hpp"

#include "Graphics.hpp"

#include "UI/UIManager.hpp"
#include "Components/CameraManager.hpp"
#include "Components/CampathManager.hpp"
#include "Graphics/Resource.hpp"
#include "Input.hpp"
#include "Mod.hpp"
#include "Types/Vertex.hpp"
#include "Utilities/MathUtils.hpp"

namespace IWXMVM::GFX
{
    void GraphicsManager::Initialize()
    {
        BufferManager::Get().Initialize();
        BufferManager::Get().AddMesh(&axis);
        BufferManager::Get().AddMesh(&camera);
        BufferManager::Get().AddMesh(&icosphere);

        for (int i = 0; i < gizmo_translate_x.vertices.size(); i++)
        {
            gizmo_translate_x.vertices[i].col = D3DCOLOR_COLORVALUE(1, 0, 0, 1);
            gizmo_translate_y.vertices[i].col = D3DCOLOR_COLORVALUE(0, 1, 0, 1);
            gizmo_translate_z.vertices[i].col = D3DCOLOR_COLORVALUE(0, 0, 1, 1);
        }
        BufferManager::Get().AddMesh(&gizmo_translate_x);
        BufferManager::Get().AddMesh(&gizmo_translate_y);
        BufferManager::Get().AddMesh(&gizmo_translate_z);

        for (int i = 0; i < gizmo_rotate_x.vertices.size(); i++)
        {
            gizmo_rotate_x.vertices[i].col = D3DCOLOR_COLORVALUE(1, 0, 0, 1);
            gizmo_rotate_y.vertices[i].col = D3DCOLOR_COLORVALUE(0, 1, 0, 1);
            gizmo_rotate_z.vertices[i].col = D3DCOLOR_COLORVALUE(0, 0, 1, 1);
        }
        BufferManager::Get().AddMesh(&gizmo_rotate_x);
        BufferManager::Get().AddMesh(&gizmo_rotate_y);
        BufferManager::Get().AddMesh(&gizmo_rotate_z);
    }

    void GraphicsManager::Uninitialize()
    {
        BufferManager::Get().Uninitialize();
    }

    glm::mat4 GetViewMatrix()
    {
        auto& camera = Components::CameraManager::Get().GetActiveCamera();
        return glm::lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetForwardVector(), 
            glm::rotateY(glm::vec3(0, 0, 1), glm::radians(camera->GetRotation().z)));
    }

    glm::mat4 GetProjectionMatrix()
    {
        auto& camera = Components::CameraManager::Get().GetActiveCamera();

        const auto aspectRatio = ImGui::GetMainViewport()->Size.x / ImGui::GetMainViewport()->Size.y;
        const auto tanHalfFovX = glm::tan(glm::radians(camera->GetFov()) * 0.5f);
        const auto tanHalfFovY = tanHalfFovX * (1.0f / aspectRatio);
        const auto fovY = glm::atan(tanHalfFovY) * 2.0f;
        const auto znear = Mod::GetGameInterface()->GetDvar("r_znear").value().value->floating_point;
        return glm::perspective(fovY, aspectRatio, znear, 10000.0f);
    }

    void GraphicsManager::SetupRenderState() const noexcept
    {
        IDirect3DDevice9* device = D3D9::GetDevice();

        device->SetPixelShader(nullptr);
        device->SetVertexShader(nullptr);
        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
        device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
        device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
        device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        device->SetRenderState(D3DRS_FOGENABLE, FALSE);
        device->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
        device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
        device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
        device->SetRenderState(D3DRS_CLIPPING, FALSE);
        device->SetRenderState(D3DRS_LIGHTING, FALSE);
        device->SetRenderState(D3DRS_COLORVERTEX, TRUE);
        device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

        const auto view = GetViewMatrix();
        const auto projection = GetProjectionMatrix();

        device->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&view));
        device->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&projection));
    }

    glm::vec3 GetMouseRay(ImVec2 mousePos, glm::mat4 projection, glm::mat4 view)
    {
        ImVec2 viewportTopLeft = 
            UI::UIManager::Get().GetUIComponent<UI::GameView>(UI::Component::Component::GameView)->GetViewportPosition();
        ImVec2 viewportSize =
            UI::UIManager::Get().GetUIComponent<UI::GameView>(UI::Component::Component::GameView)->GetViewportSize();

        mousePos.x -= viewportTopLeft.x;
        mousePos.y -= viewportTopLeft.y;

        glm::vec4 mouseClipSpace(
            mousePos.x / viewportSize.x * 2.0f - 1.0f,
            (viewportSize.y - mousePos.y) / viewportSize.y * 2.0f - 1.0f,
            -1.0f,
            1.0f
        );

        glm::vec4 mouseViewSpace = glm::inverse(projection) * mouseClipSpace;
        mouseViewSpace.z = -1.0f;
        mouseViewSpace.w = 0.0f;

        glm::vec3 mouseWorldSpace = glm::inverse(view) * mouseViewSpace;
        glm::vec3 mouseRayDirection = glm::normalize(mouseWorldSpace);
        return mouseRayDirection;
    }

    bool GraphicsManager::MouseIntersects(ImVec2 mousePos, Mesh& mesh, glm::mat4 model)
    {
        auto& camera = Components::CameraManager::Get().GetActiveCamera();
        const auto view = GetViewMatrix();
        const auto projection = GetProjectionMatrix();
        const auto mouseRayDirection = GetMouseRay(mousePos, projection, view);
        auto minDistance = std::numeric_limits<float>::max();

        for (int i = 0; i < mesh.indices.size(); i += 3)
        {
            const auto i1 = mesh.indices[i];
            const auto i2 = mesh.indices[i+1];
            const auto i3 = mesh.indices[i+2];
            const auto a = glm::vec3(model * glm::vec4(mesh.vertices[i1].pos, 1.0f));
            const auto b = glm::vec3(model * glm::vec4(mesh.vertices[i2].pos, 1.0f));
            const auto c = glm::vec3(model * glm::vec4(mesh.vertices[i3].pos, 1.0f));

            // Calculate the normal of the triangle
            const auto ab = b - a;
            const auto ac = c - a;
            const auto bc = c - b;
            const auto ca = a - c;
            const auto normal = glm::normalize(glm::cross(ab, ac));
            // Calculate the distance from the camera to the plane of the triangle
            const auto distance = glm::dot(normal, a);
            // Calculate the distance from the mouse ray to the plane of the triangle
            const auto rayDistance = (distance - glm::dot(normal, camera->GetPosition())) /
                                     glm::dot(normal, mouseRayDirection);
            if (rayDistance < 0.0f)
            {
                // The triangle is behind the camera
                continue;
            }
            // Calculate the intersection point
            const auto intersection = camera->GetPosition() + rayDistance * mouseRayDirection;
            // Check if the intersection point is inside the triangle
            const auto abp = intersection - a;
            const auto bcp = intersection - b;
            const auto cap = intersection - c;
            if (glm::dot(normal, glm::cross(ab, abp)) >= 0.0f &&
                glm::dot(normal, glm::cross(bc, bcp)) >= 0.0f &&
                glm::dot(normal, glm::cross(ca, cap)) >= 0.0f)
            {
                minDistance = std::min(minDistance, rayDistance);
            }
        }
        return minDistance < std::numeric_limits<float>::max();
    }

    std::optional<int32_t> heldAxis = std::nullopt;

    void GraphicsManager::DrawGizmoComponent(Mesh& mesh, glm::mat4 model, int32_t axisIndex)
    {
        if (heldAxis.has_value() && heldAxis.value() != axisIndex)
		{
			return;
		}

        bool mouseIntersects = MouseIntersects(ImGui::GetIO().MousePos, mesh, model);
        if (!heldAxis.has_value() && mouseIntersects)
        {
			model = model * glm::scale(glm::vec3(1, 1, 1) * 1.1f);
		}

        if (mouseIntersects && Input::KeyDown(ImGuiKey_MouseLeft))
        {
			heldAxis = axisIndex;
		}

        BufferManager::Get().DrawMesh(mesh, model);
    }

    void GraphicsManager::DrawTranslationGizmo(glm::vec3& position, glm::mat4 translation,
                                               glm::mat4 rotation)
    {
        auto scaledModel = translation * rotation * glm::scale(glm::vec3(1, 1, 1) * 3);

        if (Input::KeyUp(ImGuiKey_MouseLeft))
        {
            heldAxis = std::nullopt;
        }

        DrawGizmoComponent(gizmo_translate_x, scaledModel, 0);

        auto rotatedY = glm::rotate(scaledModel, glm::radians(90.0f), glm::vec3(0, 0, 1));
        DrawGizmoComponent(gizmo_translate_y, rotatedY, 1);

        auto rotatedZ = glm::rotate(scaledModel, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        DrawGizmoComponent(gizmo_translate_z, rotatedZ, 2);

        if (heldAxis.has_value())
        {
            auto amountX = ImGui::GetIO().MouseDelta.x * 0.1f;
            auto amountY = ImGui::GetIO().MouseDelta.y * 0.1f;
            auto delta = glm::vec3(0, 0, 0);
            delta[heldAxis.value()] = amountX + amountY;
            delta = glm::vec3(rotation * glm::vec4(delta, 1.0f));
            position += delta;
        }
    }

    void GraphicsManager::DrawRotationGizmo(glm::vec3& rotation, glm::mat4 translation)
    {
        auto scaledModel = translation * glm::scale(glm::vec3(1, 1, 1) * 3);

        if (Input::KeyUp(ImGuiKey_MouseLeft))
        {
            heldAxis = std::nullopt;
        }

        auto rotatedX = glm::rotate(scaledModel, glm::radians(-90.0f), glm::vec3(0, 0, 1));
        DrawGizmoComponent(gizmo_rotate_x, rotatedX, 0);

        DrawGizmoComponent(gizmo_rotate_y, scaledModel, 2);

        auto rotatedZ = glm::rotate(scaledModel, glm::radians(90.0f), glm::vec3(1, 0, 0));
        DrawGizmoComponent(gizmo_rotate_z, rotatedZ, 1);

        if (heldAxis.has_value())
        {
            auto amountX = ImGui::GetIO().MouseDelta.x * 0.1f;
            auto amountY = ImGui::GetIO().MouseDelta.y * 0.1f;
            auto delta = glm::vec3(0, 0, 0);
            delta[heldAxis.value()] = amountX + amountY;
            rotation += delta;
        }
    }

    enum GizmoMode
    {
        TranslateLocal,
        TranslateGlobal,
        Rotate,
        Count
    };

    std::optional<int32_t> selectedNodeId = std::nullopt;
    GizmoMode gizmoMode = GizmoMode::TranslateLocal;

    void GraphicsManager::Render()
    {
        if (ImGui::GetMainViewport()->Size.x == 0.0f || ImGui::GetMainViewport()->Size.y == 0.0f)
        {
            return;
        }

        IDirect3DDevice9* device = D3D9::GetDevice();
        HRESULT result = {};

        // Variables used for backing up and restoring D3D9 state
        IDirect3DStateBlock9* d3d9_state_block = nullptr;
        D3DMATRIX last_world = {}, last_view = {}, last_projection = {};

        // Backup the DX9 state
        result = device->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block);
        if (FAILED(result))
        {
            throw std::runtime_error("Failed to get the D3D9 state block");
        }

        // Backup the DX9 transform (DX9 documentation suggests that it is included in the StateBlock but it doesn't
        // appear to)
        device->GetTransform(D3DTS_WORLD, &last_world);
        device->GetTransform(D3DTS_VIEW, &last_view);
        device->GetTransform(D3DTS_PROJECTION, &last_projection);

        SetupRenderState();

        BufferManager::Get().BindBuffers();

        // Only draw nodes if in free/orbit mode
        const auto& activeCam = Components::CameraManager::Get().GetActiveCamera();
        const auto currentCameraMode = activeCam->GetMode();
        if (currentCameraMode == Components::Camera::Mode::Free || currentCameraMode == Components::Camera::Mode::Orbit)
        {
            // Draw axis when in orbit mode
            if (currentCameraMode == Components::Camera::Mode::Orbit)
            {
                const auto* orbitCam = reinterpret_cast<const Components::OrbitCamera*>(activeCam.get());

                const auto translate = glm::translate(orbitCam->GetOrigin());
                const auto scale = glm::scale(glm::vec3(1, 1, 1) * glm::distance(activeCam->GetPosition(), orbitCam->GetOrigin()) / 45.0f);

                device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE); // Disable depth test
                BufferManager::Get().DrawMesh(axis, translate * scale);
                device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
            }

            auto& keyframeManager = Components::KeyframeManager::Get();
            const auto& property = keyframeManager.GetProperty(Types::KeyframeablePropertyType::CampathCamera);
            auto& nodes = keyframeManager.GetKeyframes(property);

            // Iterate through all nodes and draw the camera model
            for (auto& node : nodes)
            {
                const auto translate = glm::translate(node.value.cameraData.position);
                const auto rotate = glm::eulerAngleZYX(glm::radians(node.value.cameraData.rotation.y),
                                       glm::radians(node.value.cameraData.rotation.x),
                                                       glm::radians(node.value.cameraData.rotation.z));
                auto scale = glm::scale(glm::vec3(1, 1, 1));
                
                bool mouseIntersects = MouseIntersects(ImGui::GetIO().MousePos, camera, translate * rotate);
                if (mouseIntersects && !heldAxis.has_value())
                   scale = glm::scale(glm::vec3(1, 1, 1) * 1.1f);
                BufferManager::Get().DrawMesh(camera, translate * rotate * scale);

                if (mouseIntersects && Input::KeyDown(ImGuiKey_MouseLeft) && selectedNodeId != node.id)
                {
                   selectedNodeId = node.id;
                   gizmoMode = GizmoMode::TranslateLocal;
                }

                if (mouseIntersects && selectedNodeId == node.id && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    gizmoMode = static_cast<GizmoMode>((gizmoMode + 1) % GizmoMode::Count);
                }

                if (selectedNodeId == node.id)
                {
                    switch (gizmoMode)
                    {
                        case TranslateGlobal:
                        case TranslateLocal:
                            DrawTranslationGizmo(
                                node.value.cameraData.position, translate,
                                gizmoMode == GizmoMode::TranslateLocal ? rotate : glm::eulerAngleZYX(.0f, .0f, .0f));
                			break;
                        case Rotate:
                            DrawRotationGizmo(node.value.cameraData.rotation, translate);
							break;
                    }
                }
            }

            // Drawing the path
            if (!nodes.empty())
            {
                constexpr float samplesPerUnit = 0.1f;  // Changes how many models are placed inbetween each node

                for (std::size_t i = 0; i < nodes.size() - 1; i++)
                {
                    const auto distance =
                        glm::distance(nodes[i].value.cameraData.position, nodes[i + 1].value.cameraData.position);
                    for (float t = 0.0f; t <= 1.0f; t += 1.0f / (distance * samplesPerUnit))
                    {
                        const float interpTick = nodes[i + 1].tick * t + nodes[i].tick * (1.0f - t);
                        const auto interpValue = keyframeManager.Get().Interpolate(property, interpTick);

                        const auto translate = glm::translate(interpValue.cameraData.position);
                        const auto scale = glm::scale(glm::vec3(2.1f, 2.1f, 2.1f));
                        const auto rotate = glm::eulerAngleZYX(glm::radians(interpValue.cameraData.rotation.y),
                                                               glm::radians(interpValue.cameraData.rotation.x),
                                                               glm::radians(interpValue.cameraData.rotation.z));
                        BufferManager::Get().DrawMesh(icosphere, translate * scale * rotate);
                    }
                }
            }
        }

        // Restore the DX9 transform
        device->SetTransform(D3DTS_WORLD, &last_world);
        device->SetTransform(D3DTS_VIEW, &last_view);
        device->SetTransform(D3DTS_PROJECTION, &last_projection);

        // Restore the DX9 state
        d3d9_state_block->Apply();
        d3d9_state_block->Release();
    }
}  // namespace IWXMVM