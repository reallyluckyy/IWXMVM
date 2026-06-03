#pragma once
#include "Components/CaptureManager.hpp"

namespace IWXMVM::Components::Rendering
{
    Types::RenderingFlags GetRenderingFlags();

    void SetVisibleElements(VisibleElements elements);
    void ResetVisibleElements();

    void Initialize();
}  // namespace IWXMVM::Components::Rendering