#include "StdInclude.hpp"
#include "TaskbarProgress.hpp"

namespace IWXMVM::UI
{
    ITaskbarList3* pTaskbarList = nullptr;
    HWND taskbarHWND = nullptr;
    TBPFLAG state = TBPF_NOPROGRESS;

    void TaskbarProgress::Initialize(HWND hwnd)
    {
        taskbarHWND = hwnd;
        HRESULT hr = CoInitialize(NULL);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to initialize COM library");
        }
        
        hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pTaskbarList));
        if (FAILED(hr))
        {
            CoUninitialize();
            LOG_ERROR("CoCreateInstance failed");
        }
    }

    void TaskbarProgress::Shutdown()
    {
        if (pTaskbarList)
        {
            pTaskbarList->SetProgressState(taskbarHWND, TBPF_NOPROGRESS);
            pTaskbarList->Release();
        }
        CoUninitialize();
    }

    void TaskbarProgress::SetProgressState(TBPFLAG newState)
    {
        if (state != newState) 
        {
            state = newState;
            if (pTaskbarList)
            {
                pTaskbarList->SetProgressState(taskbarHWND, state);
            }
        }
    }

    void TaskbarProgress::SetProgressValue(unsigned long long current, unsigned long long max)
    {
        if (pTaskbarList)
        {
            pTaskbarList->SetProgressValue(taskbarHWND, current, max);
        }
    }
}