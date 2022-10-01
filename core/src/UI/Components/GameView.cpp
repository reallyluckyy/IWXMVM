#include "StdInclude.hpp"
#include "GameView.hpp"

#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM::UI
{

	IDirect3DTexture9* texture = NULL;
	ImVec2 textureSize = ImVec2(0, 0);

	bool CreateTexture(ImVec2 size)
	{
		auto device = Mod::GetGameInterface()->GetD3D9Device();

		auto result = D3DXCreateTexture(device, size.x, size.y, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture);
		if (FAILED(result))
			return false;

		return true;
	}

	bool CaptureBackBuffer()
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
			RenderTarget->Release();
			return false;
		}

		result = device->StretchRect(RenderTarget, NULL, textureSurface, NULL, D3DTEXF_NONE);
		if (FAILED(result))
		{
			RenderTarget->Release();
			return false;
		}

		RenderTarget->Release();
		return true;
	}

    void GameView::Initialize()
    {
		if (!CreateTexture(PathUtils::GetWindowSize(Mod::GetGameInterface()->GetWindowHandle())))
		{
			throw std::exception("Failed to create texture for game view");
		}
    }

    void GameView::Render()
    {
		ImGui::Begin("GameView", NULL, ImGuiWindowFlags_NoScrollbar);

		auto viewportSize = ImGui::GetContentRegionMax();

		if (textureSize.x != viewportSize.x || textureSize.y != viewportSize.y)
		{
			texture->Release();
			CreateTexture(viewportSize);
		}

		if (!CaptureBackBuffer())
		{
			throw std::exception("Failed to capture game view");
		}


		ImGui::Image((void*)texture, viewportSize);

		ImGui::ShowDemoWindow();

		ImGui::End();
    }
}
