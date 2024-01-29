#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
    class Preferences : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

        void ToggleVisibility() { visible = !visible; }

       private:
        void Initialize() final;

        bool visible = false;
    };
}  // namespace IWXMVM::UI