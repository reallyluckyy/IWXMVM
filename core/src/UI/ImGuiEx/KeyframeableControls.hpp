#pragma once

#include "imgui_internal.h"

#include "Types/KeyframeableProperty.hpp"

namespace ImGuiEx::Keyframeable
{
    bool SliderFloat(const char* label, float* v, float v_min, float v_max,
                     IWXMVM::Types::KeyframeablePropertyType propertyType);
    bool SliderFloat3(const char* label, float* v, float v_min, float v_max,
                     IWXMVM::Types::KeyframeablePropertyType propertyType);
    bool ColorEdit3(const char* label, float col[3], IWXMVM::Types::KeyframeablePropertyType propertyType);
}  // namespace ImGuiEx::Keyframeable
