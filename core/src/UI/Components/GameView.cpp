#include "StdInclude.hpp"
#include "GameView.hpp"

#include "Mod.hpp"
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

	ImVec2 WindowResizing(ImVec2 window)
	{
		float aspect_ratio = 16.0f / 9.0f; // Clamp to 16:9, should use whatever struct from cod4 for other resolutions

		if (window.x / window.y > aspect_ratio) {
			// If too wide, adjust width
			window.x = window.y * aspect_ratio;
		}
		else if (window.x / window.y < aspect_ratio) {
			// If too tall, adjust height
			window.y = window.x / aspect_ratio;
		}

		return window;
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
		ImGui::Begin("GameView", NULL, ImGuiWindowFlags_NoScrollbar);
		auto viewportSize = ImGui::GetContentRegionMax();

		
		if (textureSize.x != viewportSize.x || textureSize.y != viewportSize.y)
		{
			textureSize = WindowResizing(viewportSize);
			CreateTexture(texture, WindowResizing(viewportSize));
		}

		if (!CaptureBackBuffer(texture))
		{
			throw std::exception("Failed to capture game view");
		}

		ImGui::Image((void*)texture, WindowResizing(viewportSize));

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