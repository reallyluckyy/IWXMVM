#pragma once

namespace IWXMVM::D3D9
{
	void Initialize();

	HWND FindWindowHandle();
	IDirect3DDevice9* GetDevice();
}