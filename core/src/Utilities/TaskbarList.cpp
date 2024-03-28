#include "StdInclude.hpp"
#include "TaskbarList.hpp"

namespace IWXMVM::TaskbarList
{
    TaskbarList::TaskbarList(HWND hwnd)
    {
        pTaskbarList = nullptr;
        taskbarHWND = hwnd;
        Initialize(hwnd);
    }
    void TaskbarList::Initialize(HWND hwnd)
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
    void TaskbarList::SetProgressState(TBPFLAG newState)
    {
        if (state != newState) 
        {
            state = newState;
            if (pTaskbarList)
                pTaskbarList->SetProgressState(taskbarHWND, state);
        }
    }
    void TaskbarList::SetProgressValue(unsigned long long current, unsigned long long max)
    {
        if (pTaskbarList)
            pTaskbarList->SetProgressValue(taskbarHWND, current, max);
    }
    TaskbarList::~TaskbarList()
    {
        if (pTaskbarList)
        {
            pTaskbarList->SetProgressState(taskbarHWND, TBPF_NOPROGRESS);
            pTaskbarList->Release();
        }
        CoUninitialize();
    }
}