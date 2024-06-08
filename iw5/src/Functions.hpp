#pragma once
#include "Structures.hpp"

namespace IWXMVM::IW5::Functions
{
    Structures::dvar_t* FindDvar(const std::string_view name);

    void Cbuf_AddText(std::string command);

    void CL_Demo_GetStartAndEndTime(int* start, int* end);
}  // namespace IWXMVM::IW5::Functions
