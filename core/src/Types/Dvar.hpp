#pragma once

namespace IWXMVM::Types
{
    enum struct ModuleType
    {
        BaseModule,
        SecondaryModules
    };

    struct Dvar
    {
        std::string_view name;
        union Value
        {
            float floating_point;
            uint32_t uint32;
            int32_t int32;
            float vector[4];
            const char* string;
            uint8_t color[4];
        }* value;
    };

}  // namespace IWXMVM::Types