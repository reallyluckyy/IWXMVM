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

}  // namespace IWXMVM::IW5::Functions
