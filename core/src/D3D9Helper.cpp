#include "StdInclude.hpp"
#include "D3D9Helper.hpp"

#include "MinHook.h"

#include "Events.hpp"
#include "Utilities/HookManager.hpp"
#include "Mod.hpp"

#include "UI/UIManager.hpp"

namespace IWXMVM::D3D9Helper
{
	void* vTable[119];
	IDirect3DDevice9* device = nullptr;

	typedef HRESULT(__stdcall* EndScene_t)(IDirect3DDevice9* pDevice);
	EndScene_t EndScene;
	typedef HRESULT(__stdcall* Reset_t)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	Reset_t Reset;

	HRESULT __stdcall EndScene_Hook(IDirect3DDevice9* pDevice)
	{
		// If the device pointer did not change, it's most likely a premature call to EndScene and will crash
		if (!UI::UIManager::isInitialized || UI::UIManager::needsRestart.load() && device != pDevice) {
			device = pDevice;
			UI::UIManager::Initialize(pDevice);
		}

		// In the case in which the device pointer actually did not change, the UI will be reinitialized inside OnFrame
		Events::Invoke(EventType::OnFrame);

		return EndScene(pDevice);
	}

	HRESULT __stdcall Reset_Hook(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		for (const auto& component : UI::UIManager::uiComponents)
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
		HWND hwnd = Mod::GetGameInterface()->GetWindowHandle();
		if (!hwnd) {
			throw std::runtime_error("Failed to find HWND");
		}

		IDirect3D9* d3dObj = Direct3DCreate9(D3D_SDK_VERSION);
		if (!d3dObj) {
			throw std::runtime_error("Failed to create D3D object");
		}

		IDirect3DDevice9* dummyDevice = nullptr;

		D3DPRESENT_PARAMETERS d3d_params = {};
		d3d_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3d_params.hDeviceWindow = hwnd;
		d3d_params.Windowed = true;

		// Try to create device - will fail if in fullscreen
		HRESULT result = d3dObj->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			d3d_params.hDeviceWindow,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
			&d3d_params,
			&dummyDevice
		);

		// Try again in case it's fullscreen
		if (FAILED(result) || !dummyDevice) {
			d3d_params.Windowed = false;
			result = d3dObj->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				d3d_params.hDeviceWindow,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
				&d3d_params,
				&dummyDevice
			);
		}

		// Fail again -> death
		if (FAILED(result) || !dummyDevice) {
			d3dObj->Release();
			throw std::runtime_error("Failed to create dummy D3D device");
		}

		memcpy(vTable, *(void**)dummyDevice, 119 * sizeof(void*));

		LOG_DEBUG("Created dummy D3D device");

		dummyDevice->Release();
		d3dObj->Release();
	}

	void Hook()
	{
		// TODO: move minhook initialization somewhere else
		if (MH_Initialize() != MH_OK) {
			throw std::runtime_error("Failed to initialize MinHook");
		}

		HookManager::CreateHook((std::uintptr_t)vTable[16], (std::uintptr_t)Reset_Hook, (std::uintptr_t*)&Reset);
		HookManager::CreateHook((std::uintptr_t)vTable[42], (std::uintptr_t)EndScene_Hook, (std::uintptr_t*)&EndScene);
	}

	void Initialize()
	{
		CreateDummyDevice();
		Hook();
		LOG_DEBUG("Hooked D3D9");
	}

	IDirect3DDevice9* GetDevicePtr()
	{
		return device;
	}
}