#pragma once
namespace IWXMVM::UI::TaskbarProgress
{
    void Initialize(HWND hwnd);
    void Shutdown();
    void SetProgressState(TBPFLAG);
    void SetProgressValue(unsigned long long, unsigned long long);
}