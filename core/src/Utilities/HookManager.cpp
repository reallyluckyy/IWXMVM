#include "StdInclude.hpp"
#include "PathUtils.hpp"

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

	void* CreateHook(std::uintptr_t from, std::uintptr_t to, std::size_t size, bool stolenBytes)
	{
		if (size < 5) 
		{
			throw std::runtime_error("Size too small to place hook!");
			return 0;
		}

		void* gateway = VirtualAlloc(0, size + 8, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		if (!gateway) 
		{
			throw std::runtime_error("VirtualAlloc failure. Size: " + std::to_string(size));
			return 0;
		}

		int offset1 = 0;
		int offset2 = 5 - static_cast<int>(size);

		if (stolenBytes) 
		{
			if (!memcpy(gateway, reinterpret_cast<std::uintptr_t*>(from), size)) 
			{
				throw std::runtime_error("Memcpy failure. Addresses: " + std::to_string(reinterpret_cast<std::uintptr_t>(gateway)) + ", " + std::to_string(from) + ", size: " + std::to_string(size));
				return 0;
			}

			if (*reinterpret_cast<uint8_t*>(from) == 0xE8 || *reinterpret_cast<uint8_t*>(from) == 0xE9) 
			{
				// does this work correctly?

				void* gatewayTemp = static_cast<std::uintptr_t*>(gateway) + 1;
				std::uintptr_t relativeAddress = *reinterpret_cast<std::uintptr_t*>(from + 1) + from + 5;
				relativeAddress -= (reinterpret_cast<std::uintptr_t>(gatewayTemp) - 1 + 5);

				*reinterpret_cast<std::uintptr_t*>(gatewayTemp) = relativeAddress;

				if (*reinterpret_cast<std::uintptr_t*>(gatewayTemp) != relativeAddress) 
				{
					throw std::runtime_error("Memory access failure. Addresses: " + std::to_string(from) + ", " + std::to_string(from + 1));
					return 0;
				}
			}

			offset1 = size;
			offset2 = 5;
		}

		std::uintptr_t gatewayRelativeAddress = (from - reinterpret_cast<std::uintptr_t>(gateway)) - offset2;
		*reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(gateway) + offset1) = 0xE9;
		*reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(gateway) + offset1 + 1) = gatewayRelativeAddress;

		if (*reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(gateway) + offset1) != 0xE9 || *reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(gateway) + offset1 + 1) != gatewayRelativeAddress) 
		{
			throw std::runtime_error("Memory access failure. Addresses: " + std::to_string(reinterpret_cast<std::uintptr_t>(gateway) + offset1) + ", " + std::to_string(reinterpret_cast<std::uintptr_t>(gateway) + offset1 + 1));
			return 0;
		}

		return (WriteJump(from, to, size) ? gateway : 0);
	}
}