#include "StdInclude.hpp"

#include "Graphics.hpp"

#include "Components/CameraManager.hpp"
#include "Components/CampathManager.hpp"
#include "Graphics/Resource.hpp"
#include "Input.hpp"
#include "Mod.hpp"
#include "Types/Vertex.hpp"

namespace IWXMVM::GFX
{
    void GraphicsManager::Initialize()
    {
        BufferManager::Get().Initialize();
        BufferManager::Get().AddMesh(&axis);
        BufferManager::Get().AddMesh(&camera);
        BufferManager::Get().AddMesh(&icosphere);
        icosphere.fillMode = D3DFILL_WIREFRAME;
    }

    void GraphicsManager::Uninitialize()
    {
        BufferManager::Get().Uninitialize();
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

        auto& camera = Components::CameraManager::Get().GetActiveCamera();
        
        const auto view = glm::lookAt(camera->GetPosition(), camera->GetPosition() + camera->GetForwardVector(),
                                      glm::rotateY(glm::vec3(0, 0, 1), glm::radians(camera->GetRotation().z)));

        const auto aspectRatio = ImGui::GetMainViewport()->Size.x / ImGui::GetMainViewport()->Size.y;
        const auto tanHalfFovX = glm::tan(glm::radians(camera->GetFov()) * 0.5f);
        const auto tanHalfFovY = tanHalfFovX * (1.0f / aspectRatio);
        const auto fovY = glm::atan(tanHalfFovY) * 2.0f;
        const auto znear = Mod::GetGameInterface()->GetDvar("r_znear").value().value->floating_point;
        const auto projection = glm::perspective(fovY, aspectRatio, znear, 10000.0f);

        device->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&view));
        device->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&projection));
    }

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
        const auto currentCameraMode = Components::CameraManager::Get().GetActiveCamera()->GetMode();
        if (currentCameraMode == Components::Camera::Mode::Free || currentCameraMode == Components::Camera::Mode::Orbit)
        {
            // Draw axis when in orbit mode
            if (currentCameraMode == Components::Camera::Mode::Orbit)
            {
                const auto& activeCam = Components::CameraManager::Get().GetActiveCamera();
                const auto* orbitCam = reinterpret_cast<const Components::OrbitCamera*>(activeCam.get());

                const auto translate = glm::translate(orbitCam->GetOrigin());
                const auto scale = glm::scale(glm::vec3(1, 1, 1) * glm::distance(activeCam->GetPosition(), orbitCam->GetOrigin()) / 45.0f);

                device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE); // Disable depth test
                BufferManager::Get().DrawMesh(axis, translate * scale);
                device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
            }

            const auto& campathManager = Components::CampathManager::Get();
            const auto& nodes = campathManager.GetNodes();

            // Iterate through all nodes and draw the camera model
            for (const auto& node : nodes)
            {
                const auto translate = glm::translate(node.position);
                const auto rotate = glm::eulerAngleZYX(glm::radians(node.rotation.y), glm::radians(node.rotation.x),
                                                       glm::radians(node.rotation.z));
                BufferManager::Get().DrawMesh(camera, translate * rotate);
            }

            // Drawing the path
            if (nodes.size() >= campathManager.GetRequiredNodeCount())
            {
                constexpr float samplesPerUnit = 0.1f; // Changes how many models are placed inbetween each node

                for (std::size_t i = 0; i < nodes.size() - 1; i++)
                {
                    const auto distance = glm::distance(nodes[i].position, nodes[i + 1].position);
                    for (float t = 0.0f; t <= 1.0f; t += 1.0f / (distance * samplesPerUnit))
                    {
                        const float interpTick = nodes[i + 1].tick * t + nodes[i].tick * (1.0f - t);
                        const auto interpNode = campathManager.Get().Interpolate(interpTick);

                        const auto translate = glm::translate(interpNode.position);
                        const auto scale = glm::scale(glm::vec3(2.1f, 2.1f, 2.1f));
                        const auto rotate =
                            glm::eulerAngleZYX(glm::radians(interpNode.rotation.y), glm::radians(interpNode.rotation.x),
                                               glm::radians(interpNode.rotation.z));
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