#include "StdInclude.hpp"
#include "D3D9.hpp"

#include "MinHook.h"

#include "Events.hpp"
#include "Utilities/HookManager.hpp"
#include "Mod.hpp"

#include "UI/UIManager.hpp"

namespace IWXMVM::D3D9
{
    HWND gameWindowHandle = nullptr;
    void* d3d9DeviceVTable[119];
    void* d3d9VTable[17];
    IDirect3DDevice9* device = nullptr;

    typedef HRESULT(__stdcall* EndScene_t)(IDirect3DDevice9* pDevice);
    EndScene_t EndScene;
    typedef HRESULT(__stdcall* Reset_t)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
    Reset_t Reset;
    typedef HRESULT(__stdcall* CreateDevice_t)(IDirect3D9* pInterface, UINT Adapter, D3DDEVTYPE DeviceType,
                                               HWND hFocusWindow, DWORD BehaviorFlags,
                                               D3DPRESENT_PARAMETERS* pPresentationParameters,
                                               IDirect3DDevice9** ppReturnedDeviceInterface);
    CreateDevice_t CreateDevice;

    bool CheckForOverlays(std::uintptr_t returnAddress)
    {
        static constexpr std::array overlayNames{
            "gameoverlay",  // Steam
            "discord"       // Discord
        };
        static std::array<std::uintptr_t, std::size(overlayNames)> returnAddresses{};

        for (std::size_t i = 0; i < returnAddresses.size(); ++i)
        {
            if (!returnAddresses[i])
            {
                MEMORY_BASIC_INFORMATION mbi;
                ::VirtualQuery(reinterpret_cast<LPCVOID>(returnAddress), &mbi, sizeof(MEMORY_BASIC_INFORMATION));

                char module[1024];
                ::GetModuleFileName(static_cast<HMODULE>(mbi.AllocationBase), module, sizeof(module));

                if (std::string_view{module}.find(overlayNames[i]) != std::string_view::npos)
                {
                    returnAddresses[i] = returnAddress;
                    return true;
                }
            }
            else if (returnAddresses[i] == returnAddress)
            {
                return true;
            }
        }

        return false;
    }

    HRESULT __stdcall CreateDevice_Hook(IDirect3D9* pInterface, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
                                        DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,
                                        IDirect3DDevice9** ppReturnedDeviceInterface)
    {
        LOG_DEBUG("CreateDevice called with hwnd {0:x}", (std::uintptr_t)pPresentationParameters->hDeviceWindow);

        HRESULT hr = CreateDevice(pInterface, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters,
                                  ppReturnedDeviceInterface);
        if (hr != D3D_OK)
        {
            return hr;
        }

        UI::UIManager::Get().ShutdownImGui();

        device = *ppReturnedDeviceInterface;

        UI::UIManager::Get().Initialize(device, pPresentationParameters->hDeviceWindow);

        return hr;
    }

    HRESULT __stdcall EndScene_Hook(IDirect3DDevice9* pDevice)
    {
        const std::uintptr_t returnAddress = reinterpret_cast<std::uintptr_t>(_ReturnAddress());
        if (CheckForOverlays(returnAddress))
        {
            return EndScene(pDevice);
        }

        if (!UI::UIManager::Get().IsInitialized())
        {
            device = pDevice;
            UI::UIManager::Get().Initialize(pDevice);
        }

        UI::UIManager::Get().RunImGuiFrame();

        return EndScene(pDevice);
    }

    HRESULT __stdcall Reset_Hook(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
    {
        for (const auto& component : UI::UIManager::Get().GetUIComponents())
        {
            component->Release();
        }

        ImGui_ImplDX9_InvalidateDeviceObjects();
        HRESULT hr = Reset(pDevice, pPresentationParameters);
        ImGui_ImplDX9_CreateDeviceObjects();

        return hr;
    }

    void CreateDummyDevice()
    {
        HWND hwnd = D3D9::FindWindowHandle();
        if (!hwnd)
        {
            throw std::runtime_error("Failed to find HWND");
        }

        IDirect3D9* d3dObj = Direct3DCreate9(D3D_SDK_VERSION);
        if (!d3dObj)
        {
            throw std::runtime_error("Failed to create D3D object");
        }

        IDirect3DDevice9* dummyDevice = nullptr;

        D3DPRESENT_PARAMETERS d3d_params = {};
        d3d_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3d_params.hDeviceWindow = hwnd;
        d3d_params.Windowed = true;

        // Try to create device - will fail if in fullscreen
        HRESULT result = d3dObj->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3d_params.hDeviceWindow,
                                              D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
                                              &d3d_params, &dummyDevice);

        // Try again in case it's fullscreen
        if (FAILED(result) || !dummyDevice)
        {
            d3d_params.Windowed = false;
            result = d3dObj->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3d_params.hDeviceWindow,
                                          D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
                                          &d3d_params, &dummyDevice);
        }

        // Fail again -> death
        if (FAILED(result) || !dummyDevice)
        {
            d3dObj->Release();
            throw std::runtime_error("Failed to create dummy D3D device");
        }

        memcpy(d3d9DeviceVTable, *(void**)dummyDevice, 119 * sizeof(void*));
        memcpy(d3d9VTable, *(void**)d3dObj, 17 * sizeof(void*));

        LOG_DEBUG("Created dummy D3D device");

        dummyDevice->Release();
        d3dObj->Release();
    }

    void Hook()
    {
        // TODO: move minhook initialization somewhere else
        if (MH_Initialize() != MH_OK)
        {
            throw std::runtime_error("Failed to initialize MinHook");
        }

        HookManager::CreateHook((std::uintptr_t)d3d9VTable[16], (std::uintptr_t)CreateDevice_Hook,
                                (std::uintptr_t*)&CreateDevice);
        HookManager::CreateHook((std::uintptr_t)d3d9DeviceVTable[16], (std::uintptr_t)Reset_Hook,
                                (std::uintptr_t*)&Reset);
        HookManager::CreateHook((std::uintptr_t)d3d9DeviceVTable[42], (std::uintptr_t)EndScene_Hook,
                                (std::uintptr_t*)&EndScene);
    }

    void Initialize()
    {
        CreateDummyDevice();
        Hook();
        LOG_DEBUG("Hooked D3D9");
    }

    BOOL CALLBACK CheckWindowPID(HWND hwnd, LPARAM lParam)
    {
        DWORD lpdwPID;
        GetWindowThreadProcessId(hwnd, &lpdwPID);

        // It's possible the console window will be found instead of the game one
        // So we need to check against GetConsoleWindow()
        if (lpdwPID == lParam && hwnd != GetConsoleWindow())
        {
            gameWindowHandle = hwnd;
            return FALSE;
        }

        return TRUE;
    }

    HWND FindWindowHandle()
    {
        if (EnumWindows(CheckWindowPID, (LPARAM)GetCurrentProcessId()))
        {
            LOG_CRITICAL("Failed to find the game window");
            return nullptr;
        }

        return gameWindowHandle;
    }

    IDirect3DDevice9* GetDevice()
    {
        return device;
    }
}  // namespace IWXMVM::D3D9
