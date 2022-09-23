#include "StdInclude.hpp"
#include "DemoPlaybackPatch.hpp"

namespace IWXMVM::IW3
{
	constexpr auto PATCH_SIZE = 6;
	constexpr auto PATCH_ADDRESS = 0x46C94B;

	uint8_t originalCode[32];

	void DemoPlaybackPatch::Install()
	{
		if (IsInstalled())
			return;

		DWORD curProtection;
		VirtualProtect(reinterpret_cast<LPVOID>(PATCH_ADDRESS), PATCH_SIZE, PAGE_EXECUTE_READWRITE, &curProtection);

		for (size_t i = 0; i < PATCH_SIZE; i++)
		{
			originalCode[i] = *(uint8_t*)(PATCH_ADDRESS + i);
			*(uint8_t*)(PATCH_ADDRESS + i) = 0x90;
		}

		VirtualProtect(reinterpret_cast<LPVOID>(PATCH_ADDRESS), PATCH_SIZE, curProtection, &curProtection);
	}

	void DemoPlaybackPatch::Uninstall()
	{
		DWORD curProtection;
		VirtualProtect(reinterpret_cast<LPVOID>(PATCH_ADDRESS), PATCH_SIZE, PAGE_EXECUTE_READWRITE, &curProtection);

		for (size_t i = 0; i < PATCH_SIZE; i++)
		{
			*(uint8_t*)(PATCH_ADDRESS + i) = originalCode[i];
		}

		VirtualProtect(reinterpret_cast<LPVOID>(PATCH_ADDRESS), PATCH_SIZE, curProtection, &curProtection);
	}

	bool DemoPlaybackPatch::IsInstalled()
	{
		if (*(uint8_t*)PATCH_ADDRESS == 0x90)
			return true;

		return false;
	}
}
