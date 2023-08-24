#include "StdInclude.hpp"
#include "UIManager.hpp"

#include "UIComponent.hpp"
#include "Utilities/HookManager.hpp"
#include "Mod.hpp"
#include "Events.hpp"
#include "Resources.hpp"

namespace IWXMVM::UI::UIManager
{
	bool initRequested = false;
	std::mutex mtx;

	void ShutdownImGui()
	{
		// ensuring synchronization with the render thread so that ImGui is not shutdown while rendering a frame
		std::lock_guard<std::mutex> guard{ mtx };
		LOG_DEBUG("Shutting down ImGui");

		SetWindowLongPtr(Mod::GetGameInterface()->GetWindowHandle(), GWLP_WNDPROC, (LONG_PTR)GameWndProc);
		Mod::GetGameInterface()->SetMouseMode(GameInterface::MouseMode::Passthrough);

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

	bool uiComponentsInitialized = false;
	void RunImGuiFrame()
	{
		// ensuring synchronization with the main thread so that ImGui is not shutdown while rendering a frame
		std::lock_guard<std::mutex> guard{ mtx };

		if (!isInitialized || needsRestart.load()) {
			// This handles initialization when the D3D9 device pointer is the same after a UI restart
			if (initRequested) {
				Initialize(Mod::GetGameInterface()->GetD3D9Device());
				initRequested = false;
				return;
			}
			initRequested = true;
			return;
		} else {
			initRequested = false;
		}

		try
		{
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (!uiComponentsInitialized)
			{
				uiComponentsInitialized = true;
				for (const auto& component : uiComponents)
					component->Initialize();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_F1))
			{
				hideOverlay.store(!hideOverlay.load());
			}

			if (!hideOverlay.load())
				for (const auto& component : uiComponents)
					component->Render();

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		} catch (std::exception& e)
		{
			LOG_CRITICAL("An exception occurred while rendering the IWXMVM user interface: {0}", e.what());
		} catch (...)
		{
			LOG_CRITICAL("An error occurred while rendering the IWXMVM user interface");

			// TODO: panic function
			MessageBox(NULL, "An error occurred while rendering the IWXMVM user interface", "FATAL ERROR", MB_OK);
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

	ImVec2 GetWindowSize(HWND hwnd)
	{
		RECT windowRect = {};
		GetWindowRect(hwnd, &windowRect);
		ImVec2 windowSize = {
			static_cast<float>(windowRect.right - windowRect.left),
			static_cast<float>(windowRect.bottom - windowRect.top)
		};
		return windowSize;
	}

	void SetImGuiStyle(float fontSize)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.FramePadding = { fontSize * 0.28f, fontSize * 0.28f };
		style.WindowPadding = {};
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.01f, 0.01f, 0.01f, 0.54f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.40f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.49f, 0.94f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.40f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0, 0, 0, 0);
	}

	INCBIN_EXTERN(IBMPLEX_FONT);
	void Initialize(IDirect3DDevice9* device)
	{
		try
		{
			// to avoid registering events after restarting ImGui
			if (!isInitialized) {
				LOG_DEBUG("Initializing ImGui...");
				LOG_DEBUG("Registering OnFrame listener");
				Events::RegisterListener(EventType::OnFrame, RunImGuiFrame);
			} else if (needsRestart.load()) {
				LOG_DEBUG("Reinitializing ImGui...");
			} else {
				throw std::logic_error("UIManager::Initialize called but UI is running");
				return;
			}

			LOG_DEBUG("Creating ImGui context");
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGui::StyleColorsDark();

			HWND hwnd = Mod::GetGameInterface()->GetWindowHandle();
			LOG_DEBUG("Initializing ImGui_ImplWin32 with HWND {0:x}", (uint32_t)hwnd);
			ImGui_ImplWin32_Init(hwnd);

			LOG_DEBUG("Initializing ImGui_ImplDX9 with D3D9 Device {0:x}", (uintptr_t)device);
			ImGui_ImplDX9_Init(device);

			// TODO: byte size is game dependent
			LOG_DEBUG("Hooking WndProc at {0:x}", Mod::GetGameInterface()->GetWndProc());
			GameWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)ImGuiWndProc);

			auto windowSize = GetWindowSize(hwnd);
			auto fontSize = std::floor(windowSize.x / 106.0f);

			ImGuiIO& io = ImGui::GetIO();
			io.IniFilename = NULL;
			// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.Fonts->ClearFonts();
			io.Fonts->AddFontFromMemoryTTF((void*)IBMPLEX_FONT_data, IBMPLEX_FONT_size, fontSize);
			SetImGuiStyle(fontSize);

			Mod::GetGameInterface()->SetMouseMode(GameInterface::MouseMode::Capture);
			isInitialized = true;
			needsRestart.store(false);

			LOG_INFO("Initialized UI");
		} catch (...)
		{
			throw std::runtime_error("Failed to initialize UI");
			// TODO: panic
		}
	}
}