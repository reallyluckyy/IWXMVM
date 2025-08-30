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

INCBIN_EXTERN(VERTEX_SHADER);
INCBIN_EXTERN(PIXEL_SHADER);
INCBIN_EXTERN(DEPTH_VERTEX_SHADER);
INCBIN_EXTERN(DEPTH_PIXEL_SHADER);

namespace IWXMVM::GFX
{
    void GraphicsManager::CreateGraphicsResources()
    {
        IDirect3DDevice9* device = D3D9::GetDevice();
        HRESULT hr = D3D_OK;

        ID3DXBuffer* errorMessageBuffer = nullptr;

        ID3DXBuffer* vertexShaderBuffer = nullptr;
        hr = D3DXCompileShader((LPCSTR)VERTEX_SHADER_data, VERTEX_SHADER_size, nullptr, NULL, "main",
            "vs_3_0",
            NULL,
            &vertexShaderBuffer, &errorMessageBuffer, nullptr);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to compile vertex shader: {}",
                reinterpret_cast<const char*>(errorMessageBuffer->GetBufferPointer()));
            errorMessageBuffer->Release();
            return;
        }
        hr = device->CreateVertexShader(reinterpret_cast<const DWORD*>(vertexShaderBuffer->GetBufferPointer()),
            &vertexShader);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create vertex shader");
        }
        vertexShaderBuffer->Release();

        ID3DXBuffer* pixelShaderBuffer = nullptr;
        hr = D3DXCompileShader((LPCSTR)PIXEL_SHADER_data, PIXEL_SHADER_size, nullptr, NULL, "main", "ps_3_0",
            NULL, &pixelShaderBuffer, &errorMessageBuffer, nullptr);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to compile pixel shader: {}",
                reinterpret_cast<const char*>(errorMessageBuffer->GetBufferPointer()));
            errorMessageBuffer->Release();
            return;
        }
        hr = device->CreatePixelShader(reinterpret_cast<const DWORD*>(pixelShaderBuffer->GetBufferPointer()),
            &pixelShader);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create pixel shader");
        }
        pixelShaderBuffer->Release();

        D3DVERTEXELEMENT9 decl[] = {
            {0, offsetof(Types::Vertex, pos), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
            {0, offsetof(Types::Vertex, normal), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
            {0, offsetof(Types::Vertex, col), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
            D3DDECL_END(),
        };
        hr = device->CreateVertexDeclaration(decl, &vertexDeclaration);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create vertex declaration");
        }
    }
    void GraphicsManager::DestroyGraphicsResources()
    {
        if (vertexDeclaration != nullptr)
        {
            vertexDeclaration->Release();
            vertexDeclaration = nullptr;
        }
        if (pixelShader != nullptr)
        {
            pixelShader->Release();
            pixelShader = nullptr;
        }
        if (vertexShader != nullptr)
        {
            vertexShader->Release();
            vertexShader = nullptr;
        }
    }

    void GraphicsManager::CreateDepthPassResources()
    {
        IDirect3DDevice9* device = D3D9::GetDevice();
        HRESULT hr = D3D_OK;

        constexpr Types::FSVertex squareData[] = {
            {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        };

        hr = device->CreateVertexBuffer(sizeof(squareData), D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &depthPassVertices, nullptr);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create depth pass vertex buffer");
        }

        void* tmp = nullptr;
        depthPassVertices->Lock(0, sizeof(squareData), &tmp, NULL);
        std::memcpy(tmp, squareData, sizeof(squareData));
        depthPassVertices->Unlock();

        D3DVERTEXELEMENT9 decl[] = {
            {0, offsetof(Types::FSVertex, p), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
            {0, offsetof(Types::FSVertex, uv), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
            D3DDECL_END(),
        };
        hr = device->CreateVertexDeclaration(decl, &depthPassVDecl);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create depth pass vertex declaration");
        }

        ID3DXBuffer* errorMessageBuffer = nullptr;

        ID3DXBuffer* vertexShaderBuffer = nullptr;
        hr = D3DXCompileShader((LPCSTR)DEPTH_VERTEX_SHADER_data, DEPTH_VERTEX_SHADER_size, nullptr, NULL, "main",
            "vs_3_0",
            NULL,
            &vertexShaderBuffer, &errorMessageBuffer, nullptr);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to compile depth vertex shader: {}",
                reinterpret_cast<const char*>(errorMessageBuffer->GetBufferPointer()));
            errorMessageBuffer->Release();
            return;
        }
        hr = device->CreateVertexShader(reinterpret_cast<const DWORD*>(vertexShaderBuffer->GetBufferPointer()),
            &depthPassVS);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create depth pass vertex shader");
        }
        vertexShaderBuffer->Release();

        ID3DXBuffer* pixelShaderBuffer = nullptr;
        hr = D3DXCompileShader((LPCSTR)DEPTH_PIXEL_SHADER_data, DEPTH_PIXEL_SHADER_size, nullptr, NULL, "main", "ps_3_0",
            NULL, &pixelShaderBuffer, &errorMessageBuffer, nullptr);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to compile depth pixel shader: {}",
                reinterpret_cast<const char*>(errorMessageBuffer->GetBufferPointer()));
            errorMessageBuffer->Release();
            return;
        }
        hr = device->CreatePixelShader(reinterpret_cast<const DWORD*>(pixelShaderBuffer->GetBufferPointer()),
            &depthPassPS);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create depth pass pixel shader");
        }
        pixelShaderBuffer->Release();
    }
    void GraphicsManager::DestroyDepthPassResources()
    {
        depthPassPS->Release();
        depthPassVS->Release();
        depthPassVDecl->Release();
        depthPassVertices->Release();
    }

    void GraphicsManager::Initialize()
    {
        CreateGraphicsResources();
        CreateDepthPassResources();

        BufferManager::Get().Initialize();

        BufferManager::Get().AddMesh(&axis);
        BufferManager::Get().AddMesh(&camera);
        BufferManager::Get().AddMesh(&icosphere);

        for (size_t i = 0; i < gizmo_translate_x.vertices.size(); i++)
        {
            gizmo_translate_x.vertices[i].col = D3DCOLOR_COLORVALUE(1, 0, 0, 1);
            gizmo_translate_y.vertices[i].col = D3DCOLOR_COLORVALUE(0, 1, 0, 1);
            gizmo_translate_z.vertices[i].col = D3DCOLOR_COLORVALUE(0, 0, 1, 1);
        }
        BufferManager::Get().AddMesh(&gizmo_translate_x);
        BufferManager::Get().AddMesh(&gizmo_translate_y);
        BufferManager::Get().AddMesh(&gizmo_translate_z);

        for (size_t i = 0; i < gizmo_rotate_x.vertices.size(); i++)
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
        DestroyDepthPassResources();
        DestroyGraphicsResources();
    }

    glm::mat4 GetViewMatrix()
    {
        const auto& camera = Components::CameraManager::Get().GetActiveCamera();

        const auto& position = camera->GetFinalPosition();
        const auto& rotation = camera->GetFinalRotation();

        const auto rotationMatrix =
            glm::eulerAngleZYX(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
        const glm::vec3 forwardVector = {
            rotationMatrix[0][0],
            rotationMatrix[0][1],
            rotationMatrix[0][2],
        };
        const glm::vec3 upVector = {
            rotationMatrix[2][0],
            rotationMatrix[2][1],
            rotationMatrix[2][2],
        };

        auto view = glm::lookAtLH(position, position + forwardVector, upVector);
        view[0][0] *= -1.0f;
        view[1][0] *= -1.0f;
        view[2][0] *= -1.0f;
        view[3][0] *= -1.0f;

        return view;
    }

    glm::mat4 GetProjectionMatrix()
    {
        const auto& camera = Components::CameraManager::Get().GetActiveCamera();

        const auto aspectRatio = ImGui::GetMainViewport()->Size.x / ImGui::GetMainViewport()->Size.y;
        const auto tanHalfFovX = glm::tan(glm::radians(camera->GetFov()) * 0.5f);
        const auto tanHalfFovY = tanHalfFovX * (1.0f / aspectRatio);
        const auto fovY = glm::atan(tanHalfFovY) * 2.0f;
        const auto znear = Mod::GetGameInterface()->GetDvar("r_znear").value().value->floating_point;

        return glm::perspectiveLH_ZO(fovY, aspectRatio, znear, 100000.0f);
    }

    void GraphicsManager::SetupRenderState() const noexcept
    {
        IDirect3DDevice9* device = D3D9::GetDevice();

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

        const auto identityMatrix = glm::identity<glm::mat4>();
        device->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&identityMatrix));
        device->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&identityMatrix));
        device->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&identityMatrix));

        const auto viewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();

        device->SetVertexDeclaration(vertexDeclaration);
        device->SetVertexShaderConstantF(0, reinterpret_cast<const float*>(&viewProjectionMatrix), 4);
        device->SetVertexShader(vertexShader);


        const auto& camera = Components::CameraManager::Get().GetActiveCamera();
        const auto sun = Mod::GetGameInterface()->GetSun();
        const auto filmtweaks = Mod::GetGameInterface()->GetFilmtweaks();

        const float pitch = glm::radians(-sun.direction.x);
        const float yaw = glm::radians(sun.direction.y);
        const float sunDirX = glm::cos(yaw) * glm::cos(pitch);
        const float sunDirY = glm::sin(yaw) * glm::cos(pitch);
        const float sunDirZ = glm::sin(pitch);
        const glm::vec4 sunLightDirection = {sunDirX, sunDirY, sunDirZ, 0.0f};
        const glm::vec4 sunLightColor = glm::vec4{sun.color.x, sun.color.y, sun.color.z, 0.0f} * sun.brightness;
        const glm::vec4 cameraPosition = glm::vec4(camera->GetPosition(), 0);

        glm::vec4 filmtweaksParams = {};
        if (filmtweaks.enabled)
        {
            filmtweaksParams = {filmtweaks.brightness, filmtweaks.contrast, filmtweaks.desaturation,
                                filmtweaks.invert ? -1.0f : 1.0f};
        }
        else
        {
            filmtweaksParams = {0.0f, 1.0f, 0.0f, 1.0f};
        }

        device->SetPixelShaderConstantF(12, reinterpret_cast<const float*>(&sunLightColor), 1);
        device->SetPixelShaderConstantF(13, reinterpret_cast<const float*>(&sunLightDirection), 1);
        device->SetPixelShaderConstantF(14, reinterpret_cast<const float*>(&cameraPosition), 1);
        device->SetPixelShaderConstantF(15, reinterpret_cast<const float*>(&filmtweaksParams), 1);
        device->SetPixelShader(pixelShader);
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
            1.0f - mousePos.x / viewportSize.x * 2.0f,
            1.0f - (viewportSize.y - mousePos.y) / viewportSize.y * 2.0f,
            -1.0f,
            1.0f
        );

        glm::vec4 mouseViewSpace = glm::inverse(projection) * mouseClipSpace;
        mouseViewSpace.z = -1.0f;
        mouseViewSpace.w = 0.0f;

        glm::vec3 mouseWorldSpace = glm::inverse(view) * mouseViewSpace;
        glm::vec3 mouseRayDirection = glm::normalize(mouseWorldSpace);
        return -mouseRayDirection;
    }

    bool GraphicsManager::MouseIntersects(ImVec2 mousePos, Mesh& mesh, glm::mat4 model)
    {
        auto& camera = Components::CameraManager::Get().GetActiveCamera();
        const auto view = GetViewMatrix();
        const auto projection = GetProjectionMatrix();
        const auto mouseRayDirection = GetMouseRay(mousePos, projection, view);

        for (size_t i = 0; i < mesh.indices.size(); i += 3)
        {
            const auto i1 = mesh.indices[i];
            const auto i2 = mesh.indices[i+1];
            const auto i3 = mesh.indices[i+2];
            const auto a = glm::vec3(model * glm::vec4(mesh.vertices[i1].pos, 1.0f));
            const auto b = glm::vec3(model * glm::vec4(mesh.vertices[i2].pos, 1.0f));
            const auto c = glm::vec3(model * glm::vec4(mesh.vertices[i3].pos, 1.0f));
            
            glm::vec2 baryPosition;
            float distance = 0;
            if (glm::intersectRayTriangle(camera->GetPosition(), glm::normalize(mouseRayDirection), a, b, c,
                                          baryPosition, distance))
            {
                return true;
            }
        }
        return false;
    }

    bool GraphicsManager::MouseIntersectsSphereAt(ImVec2 mousePos, glm::vec3 pos, float radius)
    {
        auto& camera = Components::CameraManager::Get().GetActiveCamera();
        const auto view = GetViewMatrix();
        const auto projection = GetProjectionMatrix();
        const auto mouseRayDirection = GetMouseRay(mousePos, projection, view);

        float distance;
        return glm::intersectRaySphere(camera->GetPosition(), glm::normalize(mouseRayDirection), pos, radius * radius, distance);
    }
    
    void GraphicsManager::DrawGizmoComponent(Mesh& mesh, glm::mat4 model, int32_t axisIndex)
    {
        if (heldAxis.has_value() && heldAxis.value().axisIndex != axisIndex)
        {
            return;
        }
        
        if (!objectHoveredThisFrame)
        {
            bool mouseIntersects = MouseIntersects(ImGui::GetIO().MousePos, mesh, model);
            objectHoveredThisFrame |= mouseIntersects;
            if (mouseIntersects && Input::KeyDown(ImGuiKey_MouseLeft))
            {
                heldAxis.emplace(TranslationGizmoData{axisIndex, glm::vec3{}});
            }
        }

        IDirect3DDevice9* device = D3D9::GetDevice();
        device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
        BufferManager::Get().DrawMesh(mesh, model, true);
        device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    }

    
    std::optional<glm::vec3> GetMousePositionOnPlane(glm::vec3 origin, glm::vec3 normal)
    {
        auto& camera = Components::CameraManager::Get().GetActiveCamera();
        const auto view = GetViewMatrix();
        const auto projection = GetProjectionMatrix();
        const auto mouseRayDirection = GetMouseRay(ImGui::GetIO().MousePos, projection, view);

        float intersectionDistance = 0;
        if (glm::intersectRayPlane(camera->GetPosition(), glm::normalize(mouseRayDirection), origin,
                                   glm::normalize(normal), intersectionDistance))
        {
            return camera->GetPosition() + intersectionDistance * mouseRayDirection;
        }

        return std::nullopt;
    }

    void GraphicsManager::DrawTranslationGizmo(glm::vec3& position, glm::mat4 translation,
                                               glm::mat4 rotation)
    {
        auto scaledModel = translation * rotation * glm::scale(glm::vec3(1, 1, 1) * 3);

        if (Input::KeyUp(ImGuiKey_MouseLeft))
        {
            heldAxis = std::nullopt;
        }

        const bool isHeld = heldAxis.has_value();
        DrawGizmoComponent(gizmo_translate_x, scaledModel, 0);

        auto rotatedY = glm::rotate(scaledModel, glm::radians(90.0f), glm::vec3(0, 0, 1));
        DrawGizmoComponent(gizmo_translate_y, rotatedY, 1);

        auto rotatedZ = glm::rotate(scaledModel, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        DrawGizmoComponent(gizmo_translate_z, rotatedZ, 2);

        if (heldAxis.has_value())
        {
            auto axisDirection = glm::vec3(0, 0, 0);
            axisDirection[heldAxis.value().axisIndex] = 1.0f;
            axisDirection = glm::vec3(rotation * glm::vec4(axisDirection, 1.0f));
            axisDirection = glm::normalize(axisDirection);

            auto& camera = Components::CameraManager::Get().GetActiveCamera();
            auto towardsCamera = glm::normalize(camera->GetPosition() - position);

            glm::vec3 bestPlaneNormal = glm::vec3(0);
            float bestDotProduct = 0;
            for (int i = 0; i < 2; i++)
            {
                auto axisNormal = glm::vec3(0, 0, 0);
                axisNormal[(heldAxis.value().axisIndex + i + 1) % 3] = 1.0f;
                axisNormal = glm::vec3(rotation * glm::vec4(axisNormal, 1.0f));
                axisNormal = glm::normalize(axisNormal);

                auto dot = glm::abs(glm::dot(axisNormal, towardsCamera));
                if (dot > bestDotProduct)
                {
                    bestDotProduct = dot;
                    bestPlaneNormal = axisNormal;
                }
            }

            auto intersection = GetMousePositionOnPlane(position, bestPlaneNormal);
            if (intersection.has_value())
            {
                if (!isHeld) 
                {
                    // store offset to account for the distance between object position and mouse position,
                    // so that the object moves relative to the mouse cursor
                    heldAxis.value().objectOffset = intersection.value() - position;
                }

                intersection.value() -= heldAxis.value().objectOffset;
                if (intersection.value() != position)
                {
                    auto dot = glm::dot(glm::normalize(intersection.value() - position), axisDirection);
                    auto distance = glm::distance(intersection.value(), position);
                    position = position + axisDirection * dot * distance;
                }
            }
        }
    }

    void GraphicsManager::DrawRotationGizmo(glm::vec3& rotation, glm::mat4 translation)
    {
        const auto rotate = glm::eulerAngleZYX(
            glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z)
        );
        const auto scaledModel = translation * rotate * glm::scale(glm::vec3(1, 1, 1) * 3);

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
            auto amountX = ImGui::GetIO().MouseDelta.x * 0.008f;
            auto amountY = ImGui::GetIO().MouseDelta.y * 0.008f;

            auto quat = glm::toQuat(rotate);

            auto selectedAxis = heldAxis.value().axisIndex;
            selectedAxis = heldAxis.value().axisIndex == 1 ? 2 : selectedAxis;
            selectedAxis = heldAxis.value().axisIndex == 2 ? 1 : selectedAxis;
            auto axis = glm::vec3(0, 0, 0);
            axis[selectedAxis] = 1;

            quat = quat * glm::angleAxis(amountX + amountY, axis);

            auto newRotation = glm::degrees(glm::eulerAngles(quat));
            rotation[0] = newRotation[1];
            rotation[1] = newRotation[2];
            rotation[2] = newRotation[0];
        }
    }

    void GraphicsManager::BuildCampathMesh()
    {
        constexpr auto lineWidth = 5.0f;
        constexpr auto lineColor = D3DCOLOR_COLORVALUE(1, 0.8f, 0, 1);

        constexpr float samplesPerUnit = 0.05f;  // Changes how many models are placed inbetween each node

        auto& keyframeManager = Components::KeyframeManager::Get();
        const auto& property = keyframeManager.GetProperty(Types::KeyframeablePropertyType::CampathCamera);
        auto& nodes = keyframeManager.GetKeyframes(property);

        campath.vertices.clear();
        campath.indices.clear();

        for (std::size_t i = 0; i < nodes.size() - 1; i++)
        {
            const auto distance =
                glm::distance(nodes[i].value.cameraData.position, nodes[i + 1].value.cameraData.position);
            for (float t = 0.0f; t <= 1.0f; t += 1.0f / (distance * samplesPerUnit))
            {
                const float interpTick = nodes[i + 1].tick * t + nodes[i].tick * (1.0f - t);
                const auto interpValue = keyframeManager.Get().Interpolate(property, interpTick);

                campath.vertices.push_back(
                    Types::Vertex{
                        .pos = interpValue.cameraData.position - glm::vec3(lineWidth / 2, 0, 0),
                        .normal = glm::vec3(0, 0, 1),
                        .col = lineColor
                    }
                );

                campath.vertices.push_back(
                    Types::Vertex{
                        .pos = interpValue.cameraData.position + glm::vec3(lineWidth / 2, 0, 0),
                        .normal = glm::vec3(0, 0, 1),
                        .col = lineColor
                    }
                );

                campath.vertices.push_back(
                    Types::Vertex{
                        .pos = interpValue.cameraData.position - glm::vec3(0, 0, lineWidth / 4),
                        .normal = glm::vec3(1, 1, 0),
                        .col = lineColor
                    }
                );

                campath.vertices.push_back(
                    Types::Vertex{
                        .pos = interpValue.cameraData.position + glm::vec3(0, 0, lineWidth / 4),
                        .normal = glm::vec3(1, 1, 0),
                        .col = lineColor
                    }
                );

                if (campath.vertices.size() > 4)
                {
                    // We create two perpendicular planes with the vertices like so:
                    //    2
                    // 0     1
                    //    3
                    // The next node would then have the vertices:
                    //    6
                    // 4     5
                    //    7
                    // and so on, which means we need these indices to create the triangles between the left/right vertices:
                    // 0 1 4
                    // 1 5 4
                    // 0 4 1
                    // 1 4 5
                    // and these for the up/down vertices:
                    // 2 3 6
                    // 3 7 6
                    // 2 6 3
                    // 3 6 7

                    std::vector<Types::Index> newIndices{
                        0, 1, 4,
                        1, 5, 4,
                        0, 4, 1,
                        1, 4, 5,
                        2, 3, 6,
                        3, 7, 6,
                        2, 6, 3,
                        3, 6, 7
                    };

                    for (auto& index : newIndices)
                    {
                        index = campath.vertices.size() - (8 - index);
                    }

                    campath.indices.insert(campath.indices.end(), newIndices.begin(), newIndices.end());
                }
            }
        }
    }

    void GraphicsManager::DrawStreamsShader(Components::PassType passType, bool onlyDrawViewmodel) const
    {
        IDirect3DDevice9* device = D3D9::GetDevice();
        IDirect3DTexture9* depthTexture = D3D9::GetDepthTexture();

        if (!depthTexture)
        {
            return;
        }

        IDirect3DStateBlock9* d3d9_state_block = nullptr;
        D3DMATRIX last_world = {}, last_view = {}, last_projection = {};

        // Backup the DX9 state
        HRESULT result = device->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block);
        if (FAILED(result))
        {
            throw std::runtime_error("Failed to get the D3D9 state block");
        }

        device->GetTransform(D3DTS_WORLD, &last_world);
        device->GetTransform(D3DTS_VIEW, &last_view);
        device->GetTransform(D3DTS_PROJECTION, &last_projection);

        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
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

        const auto identityMatrix = glm::identity<glm::mat4>();
        device->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&identityMatrix));
        device->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&identityMatrix));
        device->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&identityMatrix));

        device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

        device->SetTexture(10, depthTexture);

        const auto gameSize = ImGui::GetIO().DisplaySize;
        const float texelOffset[4] = { -1.0f / gameSize.x, 1.0f / gameSize.y, 0.0f, 0.0f };
        device->SetVertexDeclaration(depthPassVDecl);
        device->SetVertexShaderConstantF(0, reinterpret_cast<const float*>(&texelOffset), 1);
        device->SetVertexShader(depthPassVS);
        device->SetPixelShaderConstantF(
            0, 
            std::array<float, 4>{
                passType == Components::PassType::Depth ? 1.0f : 0.0f, 
                0.0f, 0.0f, 0.0f
            }.data(), 
            1
        );
        device->SetPixelShaderConstantF(
            4,
            std::array<float, 4>{
                passType == Components::PassType::Normal ? 1.0f : 0.0f,
                0.0f, 0.0f, 0.0f
            }.data(),
            1
        );
        device->SetPixelShaderConstantF(
            8,
            std::array<float, 4>{
                onlyDrawViewmodel ? 1.0f : 0.0f, 
                0.0f, 0.0f, 0.0f
            }.data(),
            1
        );
        //device->SetPixelShaderConstantF(
        //    1, std::array<float, 4>{passType == Components::PassType::Normal ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f}.data(), 1);
        //device->SetPixelShaderConstantB(6, (BOOL*)&onlyDrawViewmodel, 1);
        device->SetPixelShader(depthPassPS);
        device->SetStreamSource(0, depthPassVertices, 0, sizeof(Types::FSVertex));
        device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

        // Restore the DX9 transform
        device->SetTransform(D3DTS_WORLD, &last_world);
        device->SetTransform(D3DTS_VIEW, &last_view);
        device->SetTransform(D3DTS_PROJECTION, &last_projection);

        // Restore the DX9 state
        d3d9_state_block->Apply();
        d3d9_state_block->Release();
    }

    void GraphicsManager::DrawShaderForPassIndex(int32_t passIndex)
    {
        auto& captureSettings = Components::CaptureManager::Get().GetCaptureSettings();
        auto& pass = captureSettings.passes[passIndex];
        
        if (pass.type != Components::PassType::Default || pass.elements == Components::VisibleElements::OnlyGun)
        {
            DrawStreamsShader(
                pass.type,
                pass.elements == Components::VisibleElements::OnlyGun
            );
        }
    }

    void GraphicsManager::Render()
    {
        if (ImGui::GetMainViewport()->Size.x == 0.0f || ImGui::GetMainViewport()->Size.y == 0.0f)
        {
            return;
        }

        IDirect3DDevice9* device = D3D9::GetDevice();

        // Variables used for backing up and restoring D3D9 state
        IDirect3DStateBlock9* d3d9_state_block = nullptr;
        D3DMATRIX last_world = {}, last_view = {}, last_projection = {};

        // Backup the DX9 state
        HRESULT result = device->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block);
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

        BufferManager::Get().BindBuffers(BufferType::Static);

        objectHoveredThisFrame = false;

        const auto& activeCam = Components::CameraManager::Get().GetActiveCamera();
        const auto currentCameraMode = activeCam->GetMode();
        if (currentCameraMode == Components::Camera::Mode::Bone)
        {
            auto& boneCamera = dynamic_cast<Components::BoneCamera&>(*activeCam);
            if (boneCamera.ShowBone())
            {
                const auto& bones = Mod::GetGameInterface()->GetSupportedBoneNames();
                const std::string& selectedBoneName = bones.at(boneCamera.GetBoneIndex());
                const auto selectedPlayer = boneCamera.GetEntityId();
                const auto& boneData = Mod::GetGameInterface()->GetBoneData(selectedPlayer, selectedBoneName);

                const auto translate = glm::translate(boneData.position);
                const auto scale = glm::scale(glm::vec3(1, 1, 1) * 1.1f);
                BufferManager::Get().DrawMesh(axis, translate * glm::mat4x4(boneData.rotation) * scale, true);
            }
        }

        // Only draw nodes if in free/orbit mode
        if (currentCameraMode == Components::Camera::Mode::Free || currentCameraMode == Components::Camera::Mode::Orbit)
        {
            // Draw axis when in orbit mode
            if (currentCameraMode == Components::Camera::Mode::Orbit)
            {
                const auto* orbitCam = reinterpret_cast<const Components::OrbitCamera*>(activeCam.get());

                const auto translate = glm::translate(orbitCam->GetOrigin());
                const auto scale = glm::scale(glm::vec3(1, 1, 1) * glm::distance(activeCam->GetFinalPosition(), orbitCam->GetOrigin()) / 45.0f);

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
                
                bool mouseIntersects = MouseIntersectsSphereAt(ImGui::GetIO().MousePos, node.value.cameraData.position, 20);
                objectHoveredThisFrame |= mouseIntersects;
                if (mouseIntersects && !heldAxis.has_value())
                   scale = glm::scale(glm::vec3(1, 1, 1) * 1.1f);
                BufferManager::Get().DrawMesh(camera, translate * rotate * scale, mouseIntersects);

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
                BuildCampathMesh();

                BufferManager::Get().ClearDynamicBuffers();
                BufferManager::Get().AddMesh(&campath, BufferType::Dynamic);
                BufferManager::Get().BindBuffers(BufferType::Dynamic);
                
                BufferManager::Get().DrawMesh(campath, glm::identity<glm::mat4>(), true);
            }
        }

        auto captureMenu = UI::UIManager::Get().GetUIComponent<UI::CaptureMenu>(UI::Component::Component::CaptureMenu);
        if (captureMenu->GetDisplayPassIndex().has_value())
        {
            DrawShaderForPassIndex(captureMenu->GetDisplayPassIndex().value());
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
