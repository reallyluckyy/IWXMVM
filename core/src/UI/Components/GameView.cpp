#include "StdInclude.hpp"
#include "GameView.hpp"

#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "Utilities/PathUtils.hpp"


namespace IWXMVM::UI
{
	bool CreateTexture(IDirect3DTexture9*& texture, ImVec2 size)
	{
		if (texture != NULL)
			texture->Release();

		auto device = Mod::GetGameInterface()->GetD3D9Device();

		auto result = D3DXCreateTexture(device, size.x, size.y, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture);
		if (FAILED(result))
			return false;

		return true;
	}

	bool CaptureBackBuffer(IDirect3DTexture9* texture)
	{
		auto device = Mod::GetGameInterface()->GetD3D9Device();

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

    void GameView::Initialize()
    {
		auto scaleFactor = 0.8f;

		SetPosition(0, UIManager::uiComponents[UIManager::Component::MenuBar]->GetSize().y);
		SetSize(ImGui::GetIO().DisplaySize.x * scaleFactor, ImGui::GetIO().DisplaySize.y * scaleFactor);
		LOG_DEBUG("Initializing GameView. size.x: {}; size.y: {}", GetSize().x, GetSize().y);
    }

	void DrawTopBar() 
	{
		auto cameraManager = Mod::GetCameraManager();
		auto& currentCamera = cameraManager->GetActiveCamera();

		ImGui::Text("View:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);

		if (ImGui::BeginCombo("##gameViewCameraCombo", cameraManager->GetCameraModeLabel(currentCamera.GetMode()).data()))
		{
			for (auto cameraMode : cameraManager->GetCameraModes())
			{
				bool isSelected = currentCamera.GetMode() == cameraMode;
				if (ImGui::Selectable(cameraManager->GetCameraModeLabel(cameraMode).data(), currentCamera.GetMode() == cameraMode))
				{
					cameraManager->SetActiveCamera(cameraMode);
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (currentCamera.GetMode() == Components::Camera::Mode::FirstPerson)
		{
			ImGui::SameLine();
			ImGui::Text("Player:");

			ImGui::SameLine();
			const char* playerCameraComboItems[] = { "Player 1", "Player 2", "Player 3", "Player 4" };
			static int currentPlayerCameraComboItem = 0;
			ImGui::SetNextItemWidth(200);
			ImGui::Combo("##gameViewCameraPlayerCombo", &currentPlayerCameraComboItem, playerCameraComboItems, IM_ARRAYSIZE(playerCameraComboItems));
		}
	}

	void GameView::Render()
	{
		ImGui::SetNextWindowPos(GetPosition());
		ImGui::SetNextWindowSize(GetSize());

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
		ImGui::Begin("GameView", NULL, flags);

		if (Mod::GetGameInterface()->GetGameState() == GameInterface::GameState::InDemo)
		{
			DrawTopBar();
		}

		auto viewportSize = ImGui::GetContentRegionMax();

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

		ImGui::SetCursorPosX((viewportSize.x - textureSize.x) / 2.0f);
		ImGui::SetCursorPosY((viewportSize.y - textureSize.y) / 2.0f);
		ImGui::Image((void*)texture, textureSize);

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