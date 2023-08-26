#include "StdInclude.hpp"
#include "UIManager.hpp"

#include "UIComponent.hpp"
#include "Utilities/HookManager.hpp"
#include "Mod.hpp"
#include "Events.hpp"
#include "Input.hpp"

namespace IWXMVM::UI::UIManager
{
	bool initRequested = false;
	bool hideOverlay = false;
	std::mutex mtx;

	void ShutdownImGui()
	{
		// ensuring synchronization with the render thread so that ImGui is not shutdown while rendering a frame
		std::lock_guard<std::mutex> guard{ mtx };
		LOG_DEBUG("Shutting down ImGui");

		SetWindowLongPtr(D3D9::FindWindowHandle(), GWLP_WNDPROC, (LONG_PTR)GameWndProc);
		Mod::GetGameInterface()->SetMouseMode(Types::MouseMode::Passthrough);

		for (const auto& component : uiComponents) 
		{
			component->Release();
		}

		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	bool RestartImGui()
	{
		ShutdownImGui();
		needsRestart.store(true);

		return true;
	}

	void RunImGuiFrame()
	{
		// TODO: move this to a proper input handling place
		if (GetAsyncKeyState(0x30) == SHRT_MIN && GetAsyncKeyState(VK_CONTROL) == SHRT_MIN) 
		{
			Sleep(100);
			hideOverlay = !hideOverlay;
		}

		// ensuring synchronization with the main thread so that ImGui is not shutdown while rendering a frame
		std::lock_guard<std::mutex> guard{ mtx };

		if (!isInitialized || needsRestart.load()) {
			// This handles initialization when the D3D9 device pointer is the same after a UI restart
			if (initRequested) {
				Initialize(D3D9::GetDevice());
				initRequested = false;
				return;
			}
			initRequested = true;
			return;
		}
		else {
			initRequested = false;
		}

		try
		{
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			Input::UpdateState(ImGui::GetIO());

			if (!hideOverlay) 
			{ 
				for (const auto& component : uiComponents)
				{
					component->Render();
				}
			}

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		}
		catch (std::exception& e)
		{
			LOG_CRITICAL("An exception occurred while rendering the IWXMVM user interface: {0}", e.what());
		}
		catch (...)
		{
			LOG_CRITICAL("An error occurred while rendering the IWXMVM user interface");

			// TODO: panic function
			MessageBox( NULL, "An error occurred while rendering the IWXMVM user interface", "FATAL ERROR", MB_OK);
		}
	}

	HRESULT ImGuiWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) 
		{
			return true;
		}

		return CallWindowProc(GameWndProc, hWnd, uMsg, wParam, lParam);
	}

	void SetImGuiStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowBorderSize = 0;
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.01f, 0.01f, 0.01f, 0.54f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.40f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.49f, 0.94f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.40f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	}

	void Initialize(IDirect3DDevice9* device)
	{
		try 
		{
			// to avoid registering events after restarting ImGui
			if (!isInitialized) {
				LOG_DEBUG("Initializing ImGui...");
				LOG_DEBUG("Registering OnFrame listener");
				Events::RegisterListener(EventType::OnFrame, RunImGuiFrame);
			}
			else if (needsRestart.load()) {
				LOG_DEBUG("Reinitializing ImGui...");
			}
			else {
				throw std::logic_error("UIManager::Initialize called but UI is running");
				return;
			}

			LOG_DEBUG("Creating ImGui context");
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGui::StyleColorsDark();

			HWND hwnd = D3D9::FindWindowHandle();
			LOG_DEBUG("Initializing ImGui_ImplWin32 with HWND {0:x}", (uint32_t)hwnd);
			ImGui_ImplWin32_Init(hwnd);

			LOG_DEBUG("Initializing ImGui_ImplDX9 with D3D9 Device {0:x}", (uintptr_t)device);
			ImGui_ImplDX9_Init(device);

			// TODO: byte size is game dependent
			LOG_DEBUG("Hooking WndProc at {0:x}", Mod::GetGameInterface()->GetWndProc());
			GameWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)ImGuiWndProc);

			/*
			* these are now initialized on in the constructor!
			* 
			LOG_DEBUG("Initializing {0} UI components", uiComponents.size());

			for (auto& component : uiComponents)
			{
				component->Initialize();
			}
			*/

			SetImGuiStyle();

			Mod::GetGameInterface()->SetMouseMode(Types::MouseMode::Capture);
			isInitialized = true;
			needsRestart.store(false);

			LOG_INFO("Initialized UI");
		}
		catch (...)
		{
			throw std::runtime_error("Failed to initialize UI");
			// TODO: panic
		}
	}
}
