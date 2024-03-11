#pragma once
#include "StdInclude.hpp"

#include "UIComponent.hpp"

#include "Components/Background.hpp"
#include "Components/CaptureMenu.hpp"
#include "Components/ControlBar.hpp"
#include "Components/DebugPanel.hpp"
#include "Components/DemoLoader.hpp"
#include "Components/GameView.hpp"
#include "Components/MenuBar.hpp"
#include "Components/PrimaryTabs.hpp"
#include "Components/VisualsMenu.hpp"
#include "Components/CameraMenu.hpp"
#include "Components/KeyframeEditor.hpp"
#include "Components/ControlsMenu.hpp"
#include "Components/Preferences.hpp"
#include "Components/Readme.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace IWXMVM::UI
{
    namespace Component
    {
        enum Component
        {
            Background = 0,
            MenuBar,
            GameView,
            PrimaryTabs,
            DemoLoader,
            CameraMenu,
            VisualsMenu,
            CaptureMenu,
            ControlBar,
            DebugPanel,
            KeyframeEditor,
            ControlsMenu,
            Preferences,
            Readme,

            Count,
        };
    }

    class UIManager
    {
       public:
        static UIManager& Get()
        {
            static UIManager instance;
            return instance;
        }

        UIManager(UIManager const&) = delete;
        void operator=(UIManager const&) = delete;

        void Initialize(IDirect3DDevice9* device, HWND hwnd = nullptr);
        void ShutdownImGui();
        void RunImGuiFrame();

        bool IsControllableCameraModeSelected();

        ImVec2 GetWindowSize(HWND hwnd);
        ImVec2 GetWindowPosition(HWND hwnd);

        bool IsInitialized() const
        {
            return isInitialized;
        }

        WNDPROC GetOriginalGameWndProc() const
        {
            return originalGameWndProc;
        }

        std::unique_ptr<UIComponent> const& GetUIComponent(Component::Component component) const
        {
            return uiComponents[component];
        }

        template <typename T>
        T* const& GetUIComponent(Component::Component component) const
        {
            return dynamic_cast<T*>(uiComponents[component].get());
        }

        std::array<std::unique_ptr<UIComponent>, Component::Count> const& GetUIComponents() const
        {
            return uiComponents;
        }

        void SelectTab(Tab tab)
        {
            selectedTab = tab;
        }

        Tab GetSelectedTab() const
        {
            return selectedTab;
        }

        void ToggleOverlay()
        {
            hideOverlay = !hideOverlay;
        }

        void ToggleImGuiDemo()
        {
            showImGuiDemo = !showImGuiDemo;
        }

        void ToggleDebugPanel()
        {
            showDebugPanel = !showDebugPanel;
        }

        std::recursive_mutex& GetMutex()
        {
            static std::recursive_mutex mutex;
            return mutex;
        }

        ImFont* GetBoldFont()
		{
			return ImGui::GetIO().Fonts->Fonts[1];
		}

       private:
        UIManager()
        {
        }

        std::array<std::unique_ptr<UIComponent>, Component::Count> uiComponents = {
            std::make_unique<Background>(),  std::make_unique<MenuBar>(),     std::make_unique<GameView>(),
            std::make_unique<PrimaryTabs>(), std::make_unique<DemoLoader>(),  std::make_unique<CameraMenu>(),
            std::make_unique<VisualsMenu>(), std::make_unique<CaptureMenu>(), std::make_unique<ControlBar>(),
            std::make_unique<DebugPanel>(),  std::make_unique<KeyframeEditor>(), std::make_unique<ControlsMenu>(),
            std::make_unique<Preferences>(), std::make_unique<Readme>()
        };

        Tab selectedTab = Tab::Demos;
        bool isInitialized = false;
        bool uiComponentsInitialized = false;

        bool hideOverlay = false;
        bool showImGuiDemo = false;
        bool showDebugPanel = false;

        WNDPROC originalGameWndProc = nullptr;
    };
}  // namespace IWXMVM::UI