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

    void CL_Demo_GetStartAndEndTime(int* start, int* end)
	{
        typedef void(__cdecl * CL_Demo_GetStartAndEndTime_t)(int localClientNum, int* start, int* end);
		CL_Demo_GetStartAndEndTime_t CL_Demo_GetStartAndEndTime = (CL_Demo_GetStartAndEndTime_t)GetGameAddresses().CL_Demo_GetStartAndEndTime();

		CL_Demo_GetStartAndEndTime(0, start, end);
	}

}  // namespace IWXMVM::IW5::Functions
