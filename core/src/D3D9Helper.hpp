#pragma once

namespace IWXMVM::D3D9Helper
{
	void Initialize();

	HWND FindWindowHandle();
	IDirect3DDevice9* GetDevicePtr();
}