#pragma once

namespace IWXMVM::D3D9
{
    void Initialize();

    HWND FindWindowHandle();
    IDirect3DDevice9* GetDevice();
    bool CaptureBackBuffer(IDirect3DTexture9* texture);
    bool CreateTexture(IDirect3DTexture9*& texture, ImVec2 size);
}  // namespace IWXMVM::D3D9