#include "StdInclude.hpp"
#include "Structures.hpp"

#include "Addresses.hpp"

// TODO: We might have to split this up into several files

namespace IWXMVM::IW3::Structures
{
clientConnection_t* GetClientConnection()
{
    return (clientConnection_t*)GetGameAddresses().clientConnection();
}

clientStatic_t* GetClientStatic()
{
    return (clientStatic_t*)GetGameAddresses().clientStatic();
}

clientActive_t* GetClientActive()
{
    return (clientActive_t*)GetGameAddresses().clientActive();
}

cgs_t* GetClientGlobalsStatic()
{
    return (cgs_t*)GetGameAddresses().clientGlobalsStatic();
}

cg_s* GetClientGlobals()
{
    return (cg_s*)GetGameAddresses().clientGlobals();
}

WinMouseVars_t* GetMouseVars()
{
    return (WinMouseVars_t*)GetGameAddresses().mouseVars();
}

GfxWorld* GetGfxWorld()
{
    return (GfxWorld*)GetGameAddresses().gfxWorld();
}

dvar_s* FindDvar(const std::string_view name)
{
    const char* _name = name.data();

    typedef dvar_s*(__cdecl * Dvar_FindVar_t)();
    Dvar_FindVar_t Dvar_FindVar_Internal = (Dvar_FindVar_t)GetGameAddresses().Dvar_FindMalleableVar();

    __asm mov edi, _name
    return Dvar_FindVar_Internal();
}

std::string GetFilePath(const std::string_view demoName)
{
    auto searchpath = (searchpath_s*)GetGameAddresses().fs_searchpaths();
    while (searchpath->next)
    {
        searchpath = searchpath->next;
        if (!searchpath->dir)
            continue;

        auto path = std::filesystem::path(searchpath->dir->path);
        path.append(searchpath->dir->gamedir);
        path.append("demos");
        path.append(demoName);

        if (std::filesystem::exists(path))
            return path.string();
    }

    return "";
}

// TODO: now this should really not belong in a file called "Structures.cpp"...
void Cbuf_AddText(std::string command)
{
    LOG_DEBUG("Executing command \"{0}\"", command);

    command.append("\n");

    const char* commandString = command.c_str();
    const auto Cbuf_AddText_Address = GetGameAddresses().Cbuf_AddText();

    __asm
    {
			mov eax, commandString
			mov ecx, 0
			call Cbuf_AddText_Address
    }
}
}  // namespace IWXMVM::IW3::Structures
