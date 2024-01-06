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
#include "Components/CampathMenu.hpp"
#include "Components/KeyframeEditor.hpp"
#include "Components/ControlsMenu.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace IWXMVM::UI
{
    static constexpr std::array TIMESCALE_STEPS{0.001f, 0.002f, 0.005f, 0.01f,  0.02f, 0.025f, 0.05f,  0.075f, 0.1f,
                                                0.125f, 0.2f,   0.25f,  0.333f, 0.5f,  0.75f,  1.0f,   1.25f,  1.5f,
                                                2.0f,   5.0f,   10.0f,  20.0f,  50.0f, 100.0f, 500.0f, 1000.0f};

    namespace Component
    {
        enum Component
        {
            Background = 0,
            MenuBar,
            GameView,
            PrimaryTabs,
            DemoLoader,
            CampathMenu,
            VisualsMenu,
            CaptureMenu,
            ControlBar,
            DebugPanel,
            KeyframeEditor,
            ControlsMenu,

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

        bool IsFreecamSelected();

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

       private:
        UIManager()
        {
        }

        std::array<std::unique_ptr<UIComponent>, Component::Count> uiComponents = {
            std::make_unique<Background>(),  std::make_unique<MenuBar>(),     std::make_unique<GameView>(),
            std::make_unique<PrimaryTabs>(), std::make_unique<DemoLoader>(),  std::make_unique<CampathMenu>(),
            std::make_unique<VisualsMenu>(), std::make_unique<CaptureMenu>(), std::make_unique<ControlBar>(),
            std::make_unique<DebugPanel>(),  std::make_unique<KeyframeEditor>(), std::make_unique<ControlsMenu>()
        };

        Tab selectedTab = Tab::Demos;
        bool isInitialized = false;

        bool hideOverlay = false;
        bool showImGuiDemo = false;
        bool showDebugPanel = false;

        WNDPROC originalGameWndProc = nullptr;
    };
}  // namespace IWXMVM::UI