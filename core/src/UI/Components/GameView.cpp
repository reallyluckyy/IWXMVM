#include "StdInclude.hpp"
#include "GameView.hpp"

#include "Mod.hpp"

namespace IWXMVM::UI
{

	IDirect3DTexture9* texture = NULL;

	bool CreateTexture()
	{
		auto device = Mod::GetGameInterface()->GetD3D9Device();

		auto result = D3DXCreateTexture(device, 1280, 720, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture);
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
		if (!CreateTexture())
		{
			throw std::exception("Failed to create texture for game view");
		}
    }

    void GameView::Render()
    {
		if (!CaptureBackBuffer())
		{
			throw std::exception("Failed to capture game view");
		}

		ImGui::Begin("GameView", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Image((void*)texture, ImVec2(1280 * 0.5f, 720 * 0.5f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

		ImGui::ShowDemoWindow();

		ImGui::End();
    }
}
