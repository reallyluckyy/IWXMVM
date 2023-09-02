#include "StdInclude.hpp"
#include "Structures.hpp"

#include "Addresses.hpp"

// TODO: We might have to split this up into several files

namespace IWXMVM::IW3::Structures
{
	clientConnection_t* GetClientConnection()
	{
		return (clientConnection_t*)IW3::GetGameAddresses().clientConnection();
	}

	clientStatic_t* GetClientStatic()
	{
		return (clientStatic_t*)IW3::GetGameAddresses().clientStatic();
	}

	clientActive_t* GetClientActive()
	{
		return (clientActive_t*)IW3::GetGameAddresses().clientActive();
	}

	cgs_t* GetClientGlobalsStatic()
	{
		return (cgs_t*)IW3::GetGameAddresses().clientGlobalsStatic();
	}

	cg_s* GetClientGlobals()
	{
		return (cg_s*)IW3::GetGameAddresses().clientGlobals();
	}

	WinMouseVars_t* GetMouseVars()
	{
		return (WinMouseVars_t*)IW3::GetGameAddresses().mouseVars();
	}

	dvar_s* FindDvar(const std::string_view name)
	{
		const char* _name = name.data();

		typedef dvar_s* (__cdecl* Dvar_FindVar_t)();
		Dvar_FindVar_t Dvar_FindVar_Internal = (Dvar_FindVar_t)0x56B5D0;

		__asm mov edi, _name
		return Dvar_FindVar_Internal();
	}

	std::string GetFilePath(const std::string_view demoName)
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

	// TODO: now this should really not belong in a file called "Structures.cpp"...
	void Cbuf_AddText(std::string command)
	{
		LOG_DEBUG("Executing command \"{0}\"", command);

		command.append("\n");

		const char* commandString = command.c_str();
		const auto Cbuf_AddText_Address = 0x4F8D90;

		__asm
		{
			mov eax, commandString
			mov ecx, 0
			call Cbuf_AddText_Address
		}
	}
}
