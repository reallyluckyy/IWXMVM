#include "StdInclude.hpp"
#include "UIManager.hpp"

#include "UIComponent.hpp"
#include "Utilities/HookManager.hpp"
#include "Mod.hpp"
#include "Events.hpp"

#include "Components/Background.hpp"
#include "Components/ControlBar.hpp"
#include "Components/DebugPanel.hpp"
#include "Components/MenuBar.hpp"
#include "Components/GameView.hpp"

namespace IWXMVM::UI::UIManager
{
	std::vector<std::unique_ptr<UIComponent>> uiComponents = []() {
		std::vector<std::unique_ptr<UIComponent>> vec;

		// background should probably always come first
		vec.emplace_back(std::make_unique<Background>());
		vec.emplace_back(std::make_unique<GameView>());
		vec.emplace_back(std::make_unique<DebugPanel>());
		vec.emplace_back(std::make_unique<MenuBar>());
		vec.emplace_back(std::make_unique<ControlBar>());

		return vec;
	}();

	bool hideOverlay = false;
	int ImGuiTimeout = 0;

	bool RestartImGui()
	{
		if (ImGuiTimeout != 0)
			return false;

		QUERY_USER_NOTIFICATION_STATE pquns;
		if (FAILED(SHQueryUserNotificationState(&pquns)) || pquns == QUNS_RUNNING_D3D_FULL_SCREEN)
		{
			// 2000ms timeout so that the game can restart properly
			ImGuiTimeout = (2 * ImGui::GetIO().Framerate < 2) ? 2 : 2 * ImGui::GetIO().Framerate;
		}
		else 
		{
			// 1000ms timeout so that the game can restart properly
			ImGuiTimeout = (1 * ImGui::GetIO().Framerate < 1) ? 1 : 1 * ImGui::GetIO().Framerate;
		}

		for (const auto& component : uiComponents) 
		{
			component->Release();
		}

		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		return true;
	}

	void RunImGuiFrame()
	{
		if (ImGuiTimeout > 0) 
		{
			// the game must have executed vid_restart previously
			// here's a timeout so that the game can restart properly
			if (--ImGuiTimeout == 0) 
			{
				LOG_DEBUG("Reinitializing ImGui");
				Initialize(InitType::Reinitialize);
			}

			return;
		}

		try
		{
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// TODO: move this to a proper input handling place
			if (GetAsyncKeyState(0x30) == SHRT_MIN && GetAsyncKeyState(VK_CONTROL) == SHRT_MIN)
			{
				Sleep(100);
				hideOverlay = !hideOverlay;
			}

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

	WNDPROC GameWndProc = nullptr;
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

	void Initialize(InitType type)
	{
		try 
		{
			// to avoid registering events after restarting ImGui
			if (type == InitType::Initialize) {
				LOG_DEBUG("Registering OnFrame listener");
				Events::RegisterListener(EventType::OnFrame, RunImGuiFrame);
			}

			LOG_DEBUG("Creating ImGui context");
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGui::StyleColorsDark();

			HWND hwnd = Mod::GetGameInterface()->GetWindowHandle();;
			LOG_DEBUG("Initializing ImGui_ImplWin32 with HWND {0:x}", (uint32_t)hwnd);
			ImGui_ImplWin32_Init(hwnd);

			auto device = Mod::GetGameInterface()->GetD3D9Device();
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

			Mod::GetGameInterface()->SetMouseMode(GameInterface::MouseMode::Capture);

			LOG_INFO("Initialized UI");
		}
		catch (...)
		{
			throw std::runtime_error("Failed to initialize UI");
			// TODO: panic
		}
	}
}