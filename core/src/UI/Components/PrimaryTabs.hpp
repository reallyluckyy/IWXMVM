#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
    enum Tab
    {
        Demos,
        Visuals,
        Camera,
        Record,

        Count,
    };

    class PrimaryTabs : public UIComponent
    {
       public:
        void Initialize() final;
        void Render() final;
        void Release() final;

       private:
        void RenderSelectedTab();
    };

    void DrawInaccessibleTabWarning();
}  // namespace IWXMVM::UI