#include "StdInclude.hpp"
#include "UIManager.hpp"

#include "UIComponent.hpp"
#include "Utilities/HookManager.hpp"
#include "Mod.hpp"
#include "Events.hpp"
#include "Resources.hpp"
#include "Input.hpp"

namespace IWXMVM::UI
{
	void UIManager::ShutdownImGui()
	{
		LOG_DEBUG("Shutting down ImGui");

		SetWindowLongPtr(D3D9::FindWindowHandle(), GWLP_WNDPROC, (LONG_PTR)originalGameWndProc);
		Mod::GetGameInterface()->SetMouseMode(Types::MouseMode::Passthrough);

		for (const auto& component : uiComponents)
		{
			component->Release();
		}

		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void UIManager::RunImGuiFrame()
	{
		try
		{
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			Input::UpdateState(ImGui::GetIO());

			static bool uiComponentsInitialized = false;
			if (!uiComponentsInitialized)
			{
				for (const auto& component : GetUIComponents())
					component->Initialize();
				uiComponentsInitialized = true;
			}

			if (Input::KeyDown(ImGuiKey_F1))
			{
				ToggleOverlay();
			}

			if (Input::KeyDown(ImGuiKey_F2))
			{
				ToggleImGuiDemo();
			}

			if (Input::KeyDown(ImGuiKey_F3))
			{
				ToggleDebugPanel();
			}

			if (!hideOverlay)
			{
				GetUIComponent(Component::Background)->Render();
				GetUIComponent(Component::MenuBar)->Render();
				GetUIComponent(Component::GameView)->Render();
				GetUIComponent(Component::PrimaryTabs)->Render();
				GetUIComponent(Component::ControlBar)->Render();
			}

			if (showImGuiDemo)
			{
				ImGui::ShowDemoWindow();
			}

			if (showDebugPanel)
			{
				GetUIComponent(Component::DebugPanel)->Render();
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
			MessageBox(NULL, "An error occurred while rendering the IWXMVM user interface", "FATAL ERROR", MB_OK);
		}
	}

	HRESULT ImGuiWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		{
			return true;
		}

		return CallWindowProc(UIManager::Get().GetOriginalGameWndProc(), hWnd, uMsg, wParam, lParam);
	}

	ImVec2 GetWindowSize(HWND hwnd)
	{
		RECT windowRect = {};
		GetWindowRect(hwnd, &windowRect);
		ImVec2 windowSize =
		{
			static_cast<float>(windowRect.right - windowRect.left),
			static_cast<float>(windowRect.bottom - windowRect.top)
		};
		return windowSize;
	}

	void SetImGuiStyle(float fontSize)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.FramePadding = { fontSize * 0.28f, fontSize * 0.28f };
		style.WindowPadding = { 16, 16 };
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.01f, 0.01f, 0.01f, 0.54f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.40f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.49f, 0.94f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.40f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0, 0, 0, 0);
	}

	INCBIN_EXTERN(IBMPLEX_FONT);
	INCBIN_EXTERN(TASA_ORBITER_FONT);
	INCBIN_EXTERN(RUBIK_FONT);
	INCBIN_EXTERN(WORK_SANS_FONT);
	INCBIN_EXTERN(FA_ICONS_FONT);
	void UIManager::Initialize(IDirect3DDevice9* device, HWND hwnd)
	{
		try
		{
			LOG_DEBUG("Initializing ImGui...");

			// to avoid registering events after restarting ImGui
			if (!isInitialized)
			{
				LOG_DEBUG("Registering OnFrame listener");
				Events::RegisterListener(EventType::OnFrame, [&]() {
					RunImGuiFrame();
				});
			}

			LOG_DEBUG("Creating ImGui context");
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGui::StyleColorsDark();

			if (hwnd == nullptr)
			{
				hwnd = D3D9::FindWindowHandle();
			}
			LOG_DEBUG("Initializing ImGui_ImplWin32 with HWND {0:x}", (std::uintptr_t)hwnd);
			ImGui_ImplWin32_Init(hwnd);

			LOG_DEBUG("Initializing ImGui_ImplDX9 with D3D9 Device {0:x}", (std::uintptr_t)device);
			ImGui_ImplDX9_Init(device);

			// TODO: byte size is game dependent
			LOG_DEBUG("Hooking WndProc at {0:x}", Mod::GetGameInterface()->GetWndProc());
			originalGameWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (std::uintptr_t)ImGuiWndProc);

			auto windowSize = GetWindowSize(hwnd);
			auto fontSize = std::floor(windowSize.x / 106.0f);

			ImGuiIO& io = ImGui::GetIO();
			io.IniFilename = NULL;
			// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.Fonts->ClearFonts();

			auto RegisterFont = [&](const char* name, const uint8_t* data, size_t size, float fontSize) 
			{
				ImFontConfig fontConfig;
				fontConfig.SizePixels = fontSize;
				strcpy_s(fontConfig.Name, name);
				io.Fonts->AddFontFromMemoryTTF((void*)data, size, fontSize, &fontConfig);
				LOG_DEBUG("Registered font {0} with size {1}", name, fontSize);

				auto iconFontSize = fontSize * 2.0f / 3.0f;
				static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
				ImFontConfig icons_config;
				icons_config.MergeMode = true;
				icons_config.PixelSnapH = true;
				icons_config.GlyphMinAdvanceX = iconFontSize;
				io.Fonts->AddFontFromMemoryTTF((void*)FA_ICONS_FONT_data, FA_ICONS_FONT_size, iconFontSize, &icons_config, icons_ranges);
			};

			RegisterFont("WorkSansRegular", WORK_SANS_FONT_data, WORK_SANS_FONT_size, fontSize);
			RegisterFont("IBMPlexSans", IBMPLEX_FONT_data, IBMPLEX_FONT_size, fontSize);
			RegisterFont("TASAOrbiterDisplay", TASA_ORBITER_FONT_data, TASA_ORBITER_FONT_size, fontSize);
			RegisterFont("RubikRegular", RUBIK_FONT_data, RUBIK_FONT_size, fontSize);

			SetImGuiStyle(fontSize);

			Mod::GetGameInterface()->SetMouseMode(Types::MouseMode::Capture);
			isInitialized = true;

			LOG_INFO("Initialized UI");
		}
		catch (...)
		{
			throw std::runtime_error("Failed to initialize UI");
			// TODO: panic
		}
	}
}