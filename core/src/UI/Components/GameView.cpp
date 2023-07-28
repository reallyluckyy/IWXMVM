#include "StdInclude.hpp"
#include "GameView.hpp"

#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"

#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))

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

		result = device->StretchRect(RenderTarget, NULL, textureSurface, NULL, D3DTEXF_NONE);
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

	ImVec2 ImageClamping(ImVec2 window, float frame_height) //Clamp for texture/image
	{
		float aspectRatio = ImGui::GetIO().DisplaySize.x / ImGui::GetIO().DisplaySize.y; // Clamp ratio to refDef's width/height.

		if (window.x / window.y > aspectRatio) {
			// If too wide, adjust width
			window.x = window.y * aspectRatio;
		}
		else if (window.x / window.y < aspectRatio) {
			// If too tall, adjust height
			window.y = window.x / aspectRatio;
		}

		return ImVec2(window.x, window.y - frame_height);
	}

	void ImGuiAspectRatioClamp(ImGuiSizeCallbackData* data) //Clamp for the actual ImGui window (because resizing handle gets fucked)
	{
		float aspectRatio = ImGui::GetIO().DisplaySize.x / ImGui::GetIO().DisplaySize.y;
		float currentRatio = data->DesiredSize.x / data->DesiredSize.y;

		if (currentRatio > aspectRatio) {
			// the window is too wide, adjust height
			data->DesiredSize.y = data->DesiredSize.x / aspectRatio;
		}
		else {
			// the window is too tall, adjust width
			data->DesiredSize.x = data->DesiredSize.y * aspectRatio;
		}
	}

    void GameView::Initialize()
    {
		// since this is executed inside the constructor, it's too early to use the interface pointer!

		/*if (!CreateTexture(PathUtils::GetWindowSize(Mod::GetGameInterface()->GetWindowHandle())))
		{
			throw std::exception("Failed to create texture for game view");
		}*/
    }

	void GameView::Render()
	{
		//Window Centering
		//ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		//ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), ImGuiAspectRatioClamp);

		ImGui::Begin("GameView", NULL, ImGuiWindowFlags_NoScrollbar);
		auto viewportSize = ImGui::GetContentRegionMax();

		if (textureSize.x != viewportSize.x || textureSize.y != viewportSize.y)
		{
			textureSize = viewportSize;
			CreateTexture(texture, viewportSize);
		}

		if (!CaptureBackBuffer(texture))
		{
			throw std::exception("Failed to capture game view");
		}

		ImGui::Image((void*)texture, ImageClamping(viewportSize, ImGui::GetFrameHeight() * 2)); //Resizes image to clamp aspect ratio

		ImGui::ShowDemoWindow();

		ImGui::End();
	}

	void GameView::Release()
	{
		if (texture != NULL) {
			textureSize = ImVec2(0, 0);

			texture->Release();
			texture = NULL;
		}
	}
}