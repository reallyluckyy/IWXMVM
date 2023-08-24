#include "StdInclude.hpp"
#include "PathUtils.hpp"

#include "MinHook.h"

#include "UI/UIManager.hpp"

namespace IWXMVM::HookManager
{
	bool WriteJump(std::uintptr_t from, std::uintptr_t to, std::size_t size)
	{
		DWORD curProtection;

		if (!VirtualProtect(reinterpret_cast<LPVOID>(from), size, PAGE_EXECUTE_READWRITE, &curProtection)) 
		{
			throw std::runtime_error("VirtualProtect failure. Address: " + std::to_string(from) + ", size: " + std::to_string(size));
			return false;
		}

		std::uintptr_t relativeAddress = (to - from) - 5;
		*reinterpret_cast<uint8_t*>(from) = 0xE9;
		*reinterpret_cast<std::uintptr_t*>(from + 1) = relativeAddress;

		if (*reinterpret_cast<uint8_t*>(from) != 0xE9 || *reinterpret_cast<std::uintptr_t*>(from + 1) != relativeAddress) 
		{
			throw std::runtime_error("Memory access failure. Addresses: " + std::to_string(from) + ", " + std::to_string(from + 1));
			return false;
		}

		if (!VirtualProtect(reinterpret_cast<LPVOID>(from), size, curProtection, &curProtection)) 
		{
			throw std::runtime_error("VirtualProtect failure. Address: " + std::to_string(from) + ", size: " + std::to_string(size));
			return false;
		}

		return true;
	}

	void CreateHook(std::uintptr_t originalPtr, std::uintptr_t detourPtr, std::uintptr_t* trampolinePtr)
	{
		if (MH_CreateHook((void*)originalPtr, (void*)detourPtr, (void**)trampolinePtr) != MH_OK)
			throw std::runtime_error("Failed to create hook");
		if (MH_EnableHook((void*)originalPtr) != MH_OK)
			throw std::runtime_error("Failed to enable hook");
	}

	void Unhook()
	{
		if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK)
			throw std::runtime_error("Failed to disable hooks");
		if (MH_Uninitialize() != MH_OK)
			throw std::runtime_error("Failed to uninitialize MinHook");
	}
}