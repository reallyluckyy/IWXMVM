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
#include "Configuration/PreferencesConfiguration.hpp"

namespace IWXMVM::UI
{
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

    bool wasGizmoButtonClickedThisFrame = false;
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
            wasGizmoButtonClickedThisFrame = wasGizmoButtonClickedThisFrame || true;
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

        auto offset = ImGui::GetFontSize() * 0.833f;
        ImGui::SetCursorPos(ImVec2(offset, offset));

        auto size = ImGui::GetFontSize() * 2;
        auto buttonSize = ImVec2(size, size);
        auto buttonSpacing = ImVec2(size / 10, size / 10);
        
        DrawGizmoButton(ICON_FA_MAXIMIZE, buttonSize, GFX::GizmoMode::TranslateLocal);
        ImGui::SameLine(0, buttonSpacing.x);
        DrawGizmoButton(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, buttonSize, GFX::GizmoMode::TranslateGlobal);
        ImGui::SameLine(0, buttonSpacing.x);
        DrawGizmoButton(ICON_FA_ROTATE, buttonSize, GFX::GizmoMode::Rotate);

        if (Input::KeyUp(ImGuiKey_MouseLeft) && !graphicsManager.WasObjectHoveredThisFrame() &&
            !graphicsManager.WasObjectDraggedThisFrame() && !wasGizmoButtonClickedThisFrame)
        {
            graphicsManager.ResetSelection();
        }
    }

    void DrawKeyBox(const char* keyName)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
        ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1.0f);

        auto padding = ImGui::GetFontSize() * 0.4f;
        ImGui::BeginDisabled();
        ImGui::Button(keyName, ImVec2(ImGui::CalcTextSize(keyName).x + padding, ImGui::GetTextLineHeight()));
        ImGui::EndDisabled();

        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    void DrawKeybindEntry(const char* keyName, const char* label)
    {
        DrawKeyBox(keyName);
        ImGui::SameLine(0, ImGui::GetFontSize() * 0.5f);
        ImGui::Text(label);
    }

    void DrawKeybindsBackground(auto spacing)
    {
        auto initialPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(0, ImGui::GetCursorPosY() - spacing * 0.5f));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 0.4f));
        ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1.0f);

        ImGui::BeginDisabled();
        ImGui::Button("##gameViewKeybindsBackground", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - ImGui::GetCursorPosY()));
        ImGui::EndDisabled();
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        ImGui::SetCursorPos(initialPos);
    }

    void GameView::DrawKeybinds()
    {
        if (!PreferencesConfiguration::Get().showKeybindHints)
            return;

        auto offsetX = ImGui::GetFontSize() * 0.833f; 
        auto offsetY = ImGui::GetWindowSize().y - offsetX * 2;
        ImGui::SetCursorPos(ImVec2(offsetX, offsetY));

        auto spacing = ImGui::GetFontSize() * 1.25f;
        auto smallSpacing = ImGui::GetFontSize() * 0.2f;
        auto& config = InputConfiguration::Get();

        if (Components::CameraManager::Get().GetActiveCamera()->GetMode() == Components::Camera::Mode::Free)
        {
            DrawKeybindsBackground(spacing);
            if (HasFocus())
            {
                // TODO: what do we do if this exceeds the window?

                DrawKeyBox(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraForward)));
                ImGui::SameLine(0, smallSpacing);
                DrawKeyBox(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraLeft)));
                ImGui::SameLine(0, smallSpacing);
                DrawKeyBox(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraBackward)));
                ImGui::SameLine(0, smallSpacing);
                DrawKeyBox(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraRight)));
                ImGui::SameLine(0, smallSpacing);
                DrawKeybindEntry(ICON_FA_COMPUTER_MOUSE, "Move Camera");

                ImGui::SameLine(0, spacing);
                DrawKeyBox(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraUp)));
                ImGui::SameLine(0, smallSpacing);
                DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraDown)), "Up/Down");

                ImGui::SameLine(0, spacing);
                DrawKeybindEntry("Scroll", "Field of View");

                ImGui::SameLine(0, spacing);
                DrawKeyBox("Alt");
                ImGui::SameLine(0, smallSpacing);
                DrawKeybindEntry("Scroll", "Roll Camera");

                ImGui::SameLine(0, spacing * 1.5f);
                DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::DollyAddNode)), "Place Campath Node");

                ImGui::SameLine(0, spacing);
                DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::DollyClearNodes)), "Delete Campath");

                ImGui::SameLine(0, spacing * 1.5f);
                DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraActivate)), "Unlock Mouse");
            }
            else
            {
                DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::FreeCameraActivate)), "Activate Freecam Controls");
            }
        }
        else if (Components::CameraManager::Get().GetActiveCamera()->GetMode() == Components::Camera::Mode::Orbit)
        {
            DrawKeybindsBackground(spacing);

            DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::OrbitCameraRotate)), "Rotate Camera");

            ImGui::SameLine(0, spacing);
            DrawKeyBox("Shift");
            ImGui::SameLine(0, smallSpacing);
            DrawKeybindEntry(ImGui::GetKeyName(config.GetBoundKey(Action::OrbitCameraRotate)), "Move Camera");

            ImGui::SameLine(0, spacing);
            DrawKeybindEntry("Scroll", "Zoom");
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
        SetCursorPos(static_cast<int32_t>(viewportCenter.x), static_cast<int32_t>(viewportCenter.y));
    }

    void GameView::Initialize()
    {
        auto scaleFactor = 0.8f;

        SetPosition(0, UIManager::Get().GetUIComponent(UI::Component::MenuBar)->GetSize().y);
        SetSize(ImGui::GetIO().DisplaySize.x * scaleFactor, ImGui::GetIO().DisplaySize.y * scaleFactor);
        LOG_DEBUG("Initializing GameView. size.x: {}; size.y: {}", GetSize().x, GetSize().y);

        Events::RegisterListener(EventType::OnCameraChanged, [&]() {
            auto& currentCamera = Components::CameraManager::Get().GetActiveCamera();
            if (currentCamera->GetMode() == Components::Camera::Mode::Free)
            {
                SetHasFocus(false);
            }
        });
    }

    void GameView::DrawTopBar()
    {
        auto& cameraManager = Components::CameraManager::Get();
        auto& currentCamera = cameraManager.GetActiveCamera();

        const auto PADDING = 10;
        ImGui::SetCursorPos(ImVec2(PADDING, PADDING));

        if (Components::CaptureManager::Get().IsCapturing())
        {
            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(PADDING, PADDING * 0.5f));
            ImGui::PushFont(UIManager::Get().GetBoldFont());
            ImGui::TextColored(ImVec4(1, 0, 0, 1), ICON_FA_CIRCLE " Recording...");
            ImGui::PopFont();
            return;
        }
        
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);

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

        auto demoLabel = Mod::GetGameInterface()->GetDemoInfo().name;
        ImGui::SameLine(GetSize().x - ImGui::CalcTextSize(demoLabel.c_str()).x - PADDING);
        ImGui::Text(demoLabel.c_str());
    }

    void HandleInput()
    {
        // Only handle input if no text input is active
        if (ImGui::GetIO().WantTextInput)
            return;

        if (Components::CaptureManager::Get().IsCapturing())
            return;

        auto& cameraManager = Components::CameraManager::Get();

        if (Input::KeyDown(ImGuiKey_1))
        {
            cameraManager.SetActiveCamera(Components::Camera::Mode::FirstPerson);
        }
        else if (Input::KeyDown(ImGuiKey_2))
        {
            cameraManager.SetActiveCamera(Components::Camera::Mode::ThirdPerson);
        }
        else if (Input::KeyDown(ImGuiKey_3))
        {
            cameraManager.SetActiveCamera(Components::Camera::Mode::Free);
        }
        else if (Input::KeyDown(ImGuiKey_4))
        {
            cameraManager.SetActiveCamera(Components::Camera::Mode::Orbit);
        }
        else if (Input::KeyDown(ImGuiKey_5))
        {
            cameraManager.SetActiveCamera(Components::Camera::Mode::Dolly);
        }
        else if (Input::KeyDown(ImGuiKey_6))
        {
            cameraManager.SetActiveCamera(Components::Camera::Mode::Bone);
        }
    }

    void GameView::Render()
    {
        wasGizmoButtonClickedThisFrame = false;

        ImGui::SetNextWindowPos(GetPosition());
        ImGui::SetNextWindowSize(GetSize());

        if (HasFocus() && UIManager::Get().IsFreecamSelected())
            LockMouse();

        HandleInput();

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
            const auto& camera = Components::CameraManager::Get().GetActiveCamera();
            bool shouldHaveFocus = false;
            if (camera->GetMode() == Components::Camera::Mode::Free)
                shouldHaveFocus = Input::BindDown(Action::FreeCameraActivate);
            else
                shouldHaveFocus = ImGui::IsWindowFocused();
            SetHasFocus(HasFocus() || shouldHaveFocus);
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
            D3D9::CreateTexture(texture, textureSize);
        }

        if (!D3D9::CaptureBackBuffer(texture))
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
        if (Mod::GetGameInterface()->GetGameState() == Types::GameState::InDemo)
        {
            DrawGizmoControls();
            DrawKeybinds();
        }
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