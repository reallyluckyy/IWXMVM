#include "StdInclude.hpp"
#include "UIManager.hpp"

#include "UIComponent.hpp"
#include "Utilities/HookManager.hpp"
#include "Mod.hpp"
#include "Events.hpp"

#include "Components/ControlBar.hpp"
#include "Components/DebugPanel.hpp"
#include "Components/MenuBar.hpp"

namespace IWXMVM::UI::UIManager
{
	std::vector<UIComponent*> uiComponents
	{
		new DebugPanel(),
		new MenuBar(),
		new ControlBar(),
	};

	bool show_demo_window = true;

	void RunImGuiFrame()
	{
		try
		{
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			for (auto component : uiComponents)
			{
				component->Render();
			}

			//ImGui::ShowDemoWindow(&show_demo_window);


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

	typedef LRESULT(WINAPI * WndProc_t)(HWND, UINT, WPARAM, LPARAM);
	WndProc_t GameWndProc = nullptr;
	LRESULT WINAPI ImGuiWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse || io.WantCaptureKeyboard)
			return true;

		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return GameWndProc(hWnd, msg, wParam, lParam);
	}

	void SetImGuiStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowBorderSize = 0;
	}

	void Initialize()
	{
		try 
		{
			LOG_DEBUG("Registering OnFrame listener");
			Events::RegisterListener(EventType::OnFrame, RunImGuiFrame);

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
			GameWndProc = HookManager::CreateHook<WndProc_t>(Mod::GetGameInterface()->GetWndProc(), ImGuiWndProc, 6);

			LOG_DEBUG("Initializing {0} UI components", uiComponents.size());

			for (auto component : uiComponents)
			{
				component->Initialize();
			}

			SetImGuiStyle();

			LOG_INFO("Initialized UI");
		}
		catch (...)
		{
			throw std::runtime_error("Failed to initialize UI");
		}
	}
}
