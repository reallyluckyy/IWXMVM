#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
    class CaptureMenu : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

        std::optional<int32_t> GetDisplayPassIndex() const;

       private:
        void Initialize() final;
    };
}  // namespace IWXMVM::UI