#include "StdInclude.hpp"
#include "Structures.hpp"

// TODO: We might have to split this up into several files

namespace IWXMVM::IW3::Structures
{
	clientConnection_t* GetClientConnection()
	{
		return (clientConnection_t*)0x8F4CE0;
	}

	clientStatic_t* GetClientStatic()
	{
		return (clientStatic_t*)0x956D80;
	}

	refdef_t* GetRefDef()
	{
		return (refdef_t*)0x797600;
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

	void Cvar_SetBoolByName(const char* cvar, bool val)
	{
		static constexpr std::uintptr_t address = 0x5492CB;
	
		_asm
		{
			pushad
			mov eax, cvar
			push val
			call address
			popad
		}
	}
}
