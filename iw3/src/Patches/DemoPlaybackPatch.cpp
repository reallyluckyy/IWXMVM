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

		for (size_t i = 0; i < PATCH_SIZE; i++)
		{
			originalCode[i] = *(uint8_t*)(PATCH_ADDRESS + i);
			*(uint8_t*)(PATCH_ADDRESS + i) = 0x90;
		}
	}

	void DemoPlaybackPatch::Uninstall()
	{
		for (size_t i = 0; i < PATCH_SIZE; i++)
		{
			*(uint8_t*)(PATCH_ADDRESS + i) = originalCode[i];
		}
	}

	bool DemoPlaybackPatch::IsInstalled()
	{
		if (*(uint8_t*)PATCH_ADDRESS == 0x90)
			return true;

		return false;
	}
}
