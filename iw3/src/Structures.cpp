#include "StdInclude.hpp"
#include "Structures.hpp"

// TODO: We might have to split this up into several files

namespace IWXMVM::IW3::Structures
{
	clientConnection_t* GetClientConnection()
	{
		return (clientConnection_t*)0x8F4CE0;
	}

	clientActive_t* GetClientActive()
	{
		return (clientActive_t*)0xC5F930;
	}

	WinMouseVars_t* GetMouseVars()
	{
		return (WinMouseVars_t*)0xCC147C4;
	}

	dvar_s* FindDvar(const std::string name)
	{
		const char* _name = name.c_str();

		typedef dvar_s* (__cdecl* Dvar_FindVar_t)();
		Dvar_FindVar_t Dvar_FindVar_Internal = (Dvar_FindVar_t)0x56B5D0;

		__asm mov edi, _name
		return Dvar_FindVar_Internal();
	}

	std::string GetFilePath(const std::string demoName)
	{
		auto searchpath = (searchpath_s*)0xD5EC4DC;
		while(searchpath->next)
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
}