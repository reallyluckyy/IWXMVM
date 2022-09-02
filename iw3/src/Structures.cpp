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