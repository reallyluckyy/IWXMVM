#include "StdInclude.hpp"
#include "GameView.hpp"

#include "UI/ImGuiExtensions.hpp"
#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "Utilities/PathUtils.hpp"
#include "Events.hpp"
#include "Input.hpp"

namespace IWXMVM::UI
{
	bool CreateTexture(IDirect3DTexture9*& texture, ImVec2 size)
	{
		if (texture != NULL)
			texture->Release();

		auto device = D3D9::GetDevice();

		auto result = D3DXCreateTexture(device, size.x, size.y, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture);
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

	ImVec2 ClampImage(ImVec2 window) // Clamp for texture/image
	{
		float aspectRatio = ImGui::GetIO().DisplaySize.x / ImGui::GetIO().DisplaySize.y;

		if (window.x / window.y > aspectRatio) 
		{
			// If too wide, adjust width
			window.x = window.y * aspectRatio;
		} else if (window.x / window.y < aspectRatio) 
		{
			// If too tall, adjust height
			window.y = window.x / aspectRatio;
		}

		return ImVec2(window.x, window.y);
	}

	void GameView::LockMouse()
	{
		// calling FindWindowHandle every frame here is probably not a good idea
		auto windowPosition = UIManager::Get().GetWindowPosition(D3D9::FindWindowHandle());

		POINT cursorPosition;
		GetCursorPos(&cursorPosition);

		ImVec2 viewportCenter = ImVec2(windowPosition.x + GetPosition().x + GetSize().x / 2, windowPosition.y + GetPosition().y + GetSize().y / 2);
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

		if (ImGui::BeginCombo("##gameViewCameraCombo", cameraManager.GetCameraModeLabel(currentCamera->GetMode()).data()))
		{
			for (auto cameraMode : cameraManager.GetCameraModes())
			{
				bool isSelected = currentCamera->GetMode() == cameraMode;
				if (ImGui::Selectable(cameraManager.GetCameraModeLabel(cameraMode).data(), currentCamera->GetMode() == cameraMode))
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

			const char* playerCameraComboItems[] = { "Player 1", "Player 2", "Player 3", "Player 4" };
			static int currentPlayerCameraComboItem = 0;
			ImGui::SetNextItemWidth(200);
			ImGui::Combo("##gameViewCameraPlayerCombo", &currentPlayerCameraComboItem, playerCameraComboItems, IM_ARRAYSIZE(playerCameraComboItems));
		}

		if (HasFocus() && UIManager::Get().ViewportShouldLockMouse())
		{
			ImGui::SameLine();
			ImGui::Text("Press ESC to unlock mouse");
		}

		auto demoLabel = Mod::GetGameInterface()->GetDemoInfo().name;
		ImGui::SameLine(GetSize().x - ImGui::CalcTextSize(demoLabel.c_str()).x - PADDING);
		ImGui::Text(demoLabel.c_str());
	}

	void GameView::Render()
	{
		ImGui::SetNextWindowPos(GetPosition());
		ImGui::SetNextWindowSize(GetSize());

		if (HasFocus() && UIManager::Get().ViewportShouldLockMouse())
			LockMouse();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
		ImGui::Begin("GameView", NULL, flags);

		if (HasFocus() && Input::KeyDown(ImGuiKey_Escape))
		{
			ImGui::SetWindowFocus(NULL);
			SetHasFocus(false);
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

		ImGui::SetCursorPos(ImVec2((viewportSize.x - textureSize.x) / 2.0f, (viewportSize.y - textureSize.y) / 2.0f + topBarHeight));
		ImGui::BeginChildFrame(ImGui::GetID("gameViewportInternal"), textureSize, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
		ImGui::Image((void*)texture, textureSize);
		
		Events::Invoke(EventType::OnRenderGameView);

		SetHasFocus(ImGui::IsWindowFocused());
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
}