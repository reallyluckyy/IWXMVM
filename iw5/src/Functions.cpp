#include "StdInclude.hpp"
#include "Functions.hpp"

#include "Addresses.hpp"

namespace IWXMVM::IW5::Functions
{
    Structures::dvar_t* FindDvar(const std::string_view name)
    {
        typedef Structures::dvar_t*(__cdecl * Dvar_FindVar_t)(const char* name);
        Dvar_FindVar_t Dvar_FindVar_Internal = (Dvar_FindVar_t)GetGameAddresses().Dvar_FindDvar();

        return Dvar_FindVar_Internal(name.data());
    }

    void Cbuf_AddText(std::string command)
    {
        LOG_DEBUG("Executing command \"{0}\"", command);

        command.append("\n");

        typedef void*(__cdecl * Cbuf_AddText_t)(int a1, const char* name);
        Cbuf_AddText_t Cbuf_AddText = (Cbuf_AddText_t)GetGameAddresses().Cbuf_AddText();

        Cbuf_AddText(0, command.c_str());
    }

    void CL_Demo_GetStartAndEndTime(uint32_t* start, uint32_t* end)
	{
        typedef void(__cdecl * CL_Demo_GetStartAndEndTime_t)(int localClientNum, uint32_t* start, uint32_t* end);
		CL_Demo_GetStartAndEndTime_t CL_Demo_GetStartAndEndTime = (CL_Demo_GetStartAndEndTime_t)GetGameAddresses().CL_Demo_GetStartAndEndTime();

		CL_Demo_GetStartAndEndTime(0, start, end);
	}

        
    bool CG_DObjGetWorldBoneMatrix(Structures::centity_s* entity, int boneIndex,
                                   float* matrix, Structures::DObj* dobj, float* origin)
    {
        typedef bool(__cdecl * CG_DObjGetWorldBoneMatrix_t)(Structures::centity_s* entity, Structures::DObj* dobj,
            int boneIndex, float* matrix, float* origin);
        CG_DObjGetWorldBoneMatrix_t CG_DObjGetWorldBoneMatrix = (CG_DObjGetWorldBoneMatrix_t)GetGameAddresses().CG_DObjGetWorldBoneMatrix();
        
		return CG_DObjGetWorldBoneMatrix(entity, dobj, boneIndex, matrix, origin);
    }

    Structures::DObj* Com_GetClientDObj(int handle)
    {
        typedef Structures::DObj*(__cdecl * Com_GetClientDObj_t)(int handle);
		Com_GetClientDObj_t Com_GetClientDObj = (Com_GetClientDObj_t)GetGameAddresses().Com_GetClientDObj();

		return Com_GetClientDObj(handle);
    }

    int Scr_AllocString(const char* string)
    {
        typedef int(__cdecl * Scr_AllocString_t)(const char* s, int sys);
		Scr_AllocString_t Scr_AllocString = (Scr_AllocString_t)GetGameAddresses().Scr_AllocString();

		return Scr_AllocString(string, 1);
    }

    void HideScoreboard()
    {
        typedef void (*CG_ScoresUp_t)(int localClientNum);
        CG_ScoresUp_t CG_ScoresUp = (CG_ScoresUp_t)GetGameAddresses().CG_ScoresUp();

        CG_ScoresUp(0);

        // We need to set this to 1 in case we were in a match ending killcam
        // before the rewind happened
        float* com_codeTimescale = (float*)GetGameAddresses().com_codeTimeScale();
        *com_codeTimescale = 1.0f;
    }

    std::string GetFilePath(const std::string_view demoName)
    {
        auto searchpath = (Structures::searchpath_s*)GetGameAddresses().fs_searchpaths();
        while (searchpath->next)
        {
            searchpath = searchpath->next;
            if (!searchpath->dir)
                continue;

            auto path = std::filesystem::path(searchpath->dir->path);
            path.append(searchpath->dir->gamedir);
            path.append("demo");
            path.append(demoName);

            if (std::filesystem::exists(path))
                return path.string();
        }

        return "";
    }

}  // namespace IWXMVM::IW5::Functions
