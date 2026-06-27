#pragma once
#include "Structures.hpp"

namespace IWXMVM::IW5::Functions
{
    Structures::dvar_t* FindDvar(const std::string_view name);

    std::string GetFilePath(const std::string_view demoName);

    void Cbuf_AddText(std::string command);

    void CL_Demo_GetStartAndEndTime(uint32_t* start, uint32_t* end);

    bool CG_DObjGetWorldBoneMatrix(Structures::centity_s* entity, int boneIndex,
                                float* matrix, Structures::DObj* dobj, float* origin);

    Structures::DObj* Com_GetClientDObj(int handle);
    
    int Scr_AllocString(const char* string);

    void HideScoreboard();
}  // namespace IWXMVM::IW5::Functions
