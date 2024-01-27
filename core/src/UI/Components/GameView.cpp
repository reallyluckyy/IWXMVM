#include "StdInclude.hpp"
#include "GameView.hpp"

#include "UI/ImGuiEx/ImGuiExtensions.hpp"
#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "Utilities/PathUtils.hpp"
#include "Utilities/MathUtils.hpp"
#include "Events.hpp"
#include "Input.hpp"
#include "Graphics/Graphics.hpp"

namespace IWXMVM::UI
{
    bool CreateTexture(IDirect3DTexture9*& texture, ImVec2 size)
    {
        if (texture != NULL)
            texture->Release();

        auto device = D3D9::GetDevice();

        auto result = D3DXCreateTexture(device, size.x, size.y, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8,
                                        D3DPOOL_DEFAULT, &texture);
        if (FAILED(result))
            return false;

        return true;
    }

    bool CaptureBackBuffer(IDirect3DTexture9* texture)
    {
        auto device = D3D9::GetDevice();

        IDirect3DSurface9* RenderTarget = NULL;
        auto result = device->GetRenderTarget(0, &RenderTarget);
        if (FAILED(result))
            return false;

        IDirect3DSurface9* textureSurface;
        result = texture->GetSurfaceLevel(0, &textureSurface);
        if (FAILED(result))
        {
            textureSurface->Release();
            RenderTarget->Release();
            return false;
        }

        result = device->StretchRect(RenderTarget, NULL, textureSurface, NULL, D3DTEXF_LINEAR);
        if (FAILED(result))
        {
            textureSurface->Release();
            RenderTarget->Release();
            return false;
        }

        textureSurface->Release();
        RenderTarget->Release();
        return true;
    }

    ImVec2 ClampImage(ImVec2 window)  // Clamp for texture/image
    {
        float aspectRatio = ImGui::GetIO().DisplaySize.x / ImGui::GetIO().DisplaySize.y;

        if (window.x / window.y > aspectRatio)
        {
            // If too wide, adjust width
            window.x = window.y * aspectRatio;
        }
        else if (window.x / window.y < aspectRatio)
        {
            // If too tall, adjust height
            window.y = window.x / aspectRatio;
        }

        return ImVec2(window.x, window.y);
    }

    void DrawGizmoButton(const char* label, ImVec2 size, GFX::GizmoMode gizmoMode)
    {
        auto& graphicsManager = GFX::GraphicsManager::Get();
        if (graphicsManager.GetGizmoMode() == gizmoMode)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        if (ImGui::Button(label, size))
        {
            if (graphicsManager.GetGizmoMode() == gizmoMode)
                ImGui::PopStyleColor();

            graphicsManager.SetGizmoMode(gizmoMode);
        }
        else
        {
            if (graphicsManager.GetGizmoMode() == gizmoMode)
                ImGui::PopStyleColor();
        }
    }

    void GameView::DrawGizmoControls()
    {
        auto& graphicsManager = GFX::GraphicsManager::Get();
        if (!graphicsManager.GetSelectedNodeId().has_value())
        {
            return;
        }

        ImGui::SetCursorPos(ImVec2(20, 20));

        auto buttonSize = ImVec2(50, 50);
        auto buttonSpacing = ImVec2(5, 5);
        
        DrawGizmoButton(ICON_FA_MAXIMIZE, buttonSize, GFX::GizmoMode::TranslateLocal);
        ImGui::SameLine(0, buttonSpacing.x);
        DrawGizmoButton(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, buttonSize, GFX::GizmoMode::TranslateGlobal);
        ImGui::SameLine(0, buttonSpacing.x);
        DrawGizmoButton(ICON_FA_ROTATE, buttonSize, GFX::GizmoMode::Rotate);
    }

    void DrawKey(const char* keyName)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
        ImGui::Button(keyName, ImVec2(ImGui::CalcTextSize(keyName).x + 10, ImGui::GetTextLineHeight()));
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    void DrawKeybindEntry(const char* keyName, const char* label)
	{
        DrawKey(keyName);
        ImGui::SameLine(0, 12);
        ImGui::Text(label);
	}

    void GameView::DrawKeybinds()
    {
        ImGui::SetCursorPos(ImVec2(20, ImGui::GetWindowSize().y - 40));

        auto spacing = 30;
        auto smallSpacing = 5;
        if (Components::CameraManager::Get().GetActiveCamera()->GetMode() == Components::Camera::Mode::Free)
        {
            auto& config = InputConfiguration::Get();
            if (HasFocus())
            {
                DrawKey(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraForward)));
                ImGui::SameLine(0, smallSpacing);
                DrawKey(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraLeft)));
                ImGui::SameLine(0, smallSpacing);
                DrawKey(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraBackward)));
                ImGui::SameLine(0, smallSpacing);
                DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraRight)), "Move Camera");

                ImGui::SameLine(0, spacing);
                DrawKey(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraUp)));
                ImGui::SameLine(0, smallSpacing);
                DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraDown)), "Up/Down");

                ImGui::SameLine(0, spacing);
                DrawKeybindEntry("Scroll", "Change FOV");

                ImGui::SameLine(0, spacing);
                DrawKey("Alt");
                ImGui::SameLine(0, smallSpacing);
                DrawKeybindEntry("Scroll", "Roll Camera");

                ImGui::SameLine(0, spacing);
                DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::DollyAddNode)), "Place Campath Node");

                ImGui::SameLine(0, spacing);
                DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::DollyClearNodes)), "Delete Campath");
            }
        }
    }

    void GameView::LockMouse()
    {
        // calling FindWindowHandle every frame here is probably not a good idea
        auto windowPosition = UIManager::Get().GetWindowPosition(D3D9::FindWindowHandle());

        POINT cursorPosition;
        GetCursorPos(&cursorPosition);

        ImVec2 viewportCenter = ImVec2(glm::floor(windowPosition.x + GetPosition().x + GetSize().x / 2),
                                       glm::floor(windowPosition.y + GetPosition().y + GetSize().y / 2));
        ImGui::GetIO().MousePosPrev += ImVec2(viewportCenter.x - cursorPosition.x, viewportCenter.y - cursorPosition.y);
        SetCursorPos(viewportCenter.x, viewportCenter.y);
    }

    void GameView::Initialize()
    {
        auto scaleFactor = 0.8f;

        SetPosition(0, UIManager::Get().GetUIComponent(UI::Component::MenuBar)->GetSize().y);
        SetSize(ImGui::GetIO().DisplaySize.x * scaleFactor, ImGui::GetIO().DisplaySize.y * scaleFactor);
        LOG_DEBUG("Initializing GameView. size.x: {}; size.y: {}", GetSize().x, GetSize().y);
    }

    void GameView::DrawTopBar()
    {
        auto& cameraManager = Components::CameraManager::Get();
        auto& currentCamera = cameraManager.GetActiveCamera();

        const auto PADDING = 10;

        ImGui::SetCursorPos(ImVec2(PADDING, PADDING));

        ImGui::SetNextItemWidth(300);

        if (ImGui::BeginCombo("##gameViewCameraCombo",
                              cameraManager.GetCameraModeLabel(currentCamera->GetMode()).data()))
        {
            for (auto cameraMode : cameraManager.GetCameraModes())
            {
                bool isSelected = currentCamera->GetMode() == cameraMode;
                if (ImGui::Selectable(cameraManager.GetCameraModeLabel(cameraMode).data(),
                                      currentCamera->GetMode() == cameraMode))
                {
                    cameraManager.SetActiveCamera(cameraMode);
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (currentCamera->GetMode() == Components::Camera::Mode::FirstPerson)
        {
            ImGui::SameLine();

            const char* playerCameraComboItems[] = {"Player 1", "Player 2", "Player 3", "Player 4"};
            static int currentPlayerCameraComboItem = 0;
            ImGui::SetNextItemWidth(200);
            ImGui::Combo("##gameViewCameraPlayerCombo", &currentPlayerCameraComboItem, playerCameraComboItems,
                         IM_ARRAYSIZE(playerCameraComboItems));
        }
        else if (currentCamera->GetMode() == Components::Camera::Mode::Bone)
        {
            ImGui::SameLine();

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Entity");
            ImGui::SameLine();

            auto boneCamera = static_cast<Components::BoneCamera*>(currentCamera.get());
            ImGui::SetNextItemWidth(200);
            auto entities = Mod::GetGameInterface()->GetEntities();
            if (ImGui::BeginCombo("##gameViewBoneCameraTargetCombo", entities[boneCamera->GetEntityId()].ToString().c_str()))
            {
                for (int i = 0; i < entities.size(); i++)
                {
                    if (entities[i].type == Types::EntityType::Unsupported)
                        continue;

                    bool isSelected = boneCamera->GetEntityId() == i;
                    if (ImGui::Selectable(entities[i].ToString().c_str(), boneCamera->GetEntityId() == i))
                    {
                        boneCamera->GetEntityId() = i;
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();

            const auto& supportedBoneNames = Mod::GetGameInterface()->GetSupportedBoneNames();

            ImGui::SetNextItemWidth(200);
            if (ImGui::BeginCombo("##gameViewBoneCameraBoneCombo",
                                  supportedBoneNames[boneCamera->GetBoneIndex()].c_str()))
            {
                for (int i = 0; i < supportedBoneNames.size(); i++)
                {
                    bool isSelected = boneCamera->GetBoneIndex() == i;
                    if (ImGui::Selectable(supportedBoneNames[i].data(), boneCamera->GetBoneIndex() == i))
                    {
                        boneCamera->GetBoneIndex() = i;
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();

            const auto& boneData = Mod::GetGameInterface()->GetBoneData(
                boneCamera->GetEntityId(), supportedBoneNames.at(boneCamera->GetBoneIndex()));
            if (boneData.id == -1)
            {
                ImGui::Text(ICON_FA_TRIANGLE_EXCLAMATION " Bone not found on entity");
            }
            else
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Offset");
                ImGui::SameLine();

                ImGui::SetNextItemWidth(200);
                ImGui::DragFloat3("##gameViewBoneCameraOffset", &boneCamera->GetPositionOffset().x, 1, -10000, 10000,
                                  "%.1f");

                ImGui::SameLine();

                ImGui::AlignTextToFramePadding();
                ImGui::Text("Rotation");
                ImGui::SameLine();

                ImGui::SetNextItemWidth(200);
                ImGui::DragFloat3("##gameViewBoneCameraRotation", &boneCamera->GetRotationOffset().x, 1, -180, 180,
                                  "%.1f");

                ImGui::SameLine();

                ImGui::AlignTextToFramePadding();
                ImGui::Text("FOV");
                ImGui::SameLine();

                ImGui::SetNextItemWidth(50);
                ImGui::DragFloat("##gameViewBoneCameraFOV", &boneCamera->GetFov(), 1, 1, 180, "%.0f");
            }
        }


        if (UIManager::Get().IsFreecamSelected())
        {
            ImGui::SameLine();

            if (HasFocus())
                ImGui::Text("Press ESC to unlock mouse");
            else
                ImGui::Text("Press %s to move freecam", ImGui::GetKeyName(InputConfiguration::Get().GetBoundKey(Action::FreeCameraActivate)));
        }

        auto demoLabel = Mod::GetGameInterface()->GetDemoInfo().name;
        ImGui::SameLine(GetSize().x - ImGui::CalcTextSize(demoLabel.c_str()).x - PADDING);
        ImGui::Text(demoLabel.c_str());
    }

    void GameView::Render()
    {
        ImGui::SetNextWindowPos(GetPosition());
        ImGui::SetNextWindowSize(GetSize());

        if (HasFocus() && UIManager::Get().IsFreecamSelected())
            LockMouse();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav |
                                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("GameView", NULL, flags);

        auto isGameFocused = D3D9::FindWindowHandle() == GetForegroundWindow();
        if (HasFocus() && (Input::KeyDown(ImGuiKey_Escape) || Input::BindDown(Action::FreeCameraActivate) || !isGameFocused))
        {
            SetHasFocus(false);
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        }
        else
        {
            SetHasFocus(HasFocus() || Input::BindDown(Action::FreeCameraActivate));
        }

        auto currentPos = ImGui::GetWindowPos();
        auto currentSize = ImGui::GetWindowSize();

        if (currentSize.x != GetSize().x && currentPos.x == GetPosition().x)
        {
            // right side resizing
            SetSizeX(currentSize.x);
        }
        if (currentSize.y != GetSize().y && currentPos.y == GetPosition().y)
        {
            // bottom side resizing
            SetSizeY(currentSize.y);
        }

        auto topBarHeight = 0.0f;
        if (Mod::GetGameInterface()->GetGameState() == Types::GameState::InDemo)
        {
            DrawTopBar();
            topBarHeight = ImGui::GetItemRectSize().y + 30;
        }

        auto viewportSize = ImGui::GetContentRegionMax();
        viewportSize.y -= topBarHeight;

        auto newTextureSize = ClampImage(viewportSize);
        if (textureSize.x != newTextureSize.x || textureSize.y != newTextureSize.y)
        {
            textureSize = newTextureSize;
            CreateTexture(texture, textureSize);
        }

        if (!CaptureBackBuffer(texture))
        {
            throw std::exception("Failed to capture game view");
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        ImGui::SetCursorPos(
            ImVec2((viewportSize.x - textureSize.x) / 2.0f, (viewportSize.y - textureSize.y) / 2.0f + topBarHeight));
        ImGui::BeginChildFrame(ImGui::GetID("gameViewportInternal"), textureSize,
                               ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoScrollbar);
        this->viewportPosition = ImGui::GetCurrentWindow()->DC.CursorPos;
        this->viewportSize = textureSize;
        ImGui::Image((void*)texture, textureSize);
        Events::Invoke(EventType::OnRenderGameView);
        DrawGizmoControls();
        DrawKeybinds();

        ImGui::EndChildFrame();

        ImGui::PopStyleVar();

        ImGui::End();

        ImGui::PopStyleVar();
    }

    void GameView::Release()
    {
        if (texture != NULL)
        {
            textureSize = ImVec2(0, 0);

            texture->Release();
            texture = NULL;
        }
    }
}  // namespace IWXMVM::UI