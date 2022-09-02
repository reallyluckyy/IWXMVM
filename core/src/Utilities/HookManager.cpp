#include "StdInclude.hpp"
#include "PathUtils.hpp"

namespace IWXMVM::HookManager
{
	void WriteJump(std::uintptr_t from, const std::uintptr_t to, std::size_t len)
	{
		const auto jumpOffset = to - from - 5;

		*(std::uint8_t*)from = 0xE9;
		std::memcpy(reinterpret_cast<void*>(from + 1), &jumpOffset, sizeof(jumpOffset));

		if (len > 5)
		{
			*(std::uint8_t*)from = 0xE9;
			for (uint32_t i = 0; i < len - 5; i++)
			{
				*(uint8_t*)(from + 5 + i) = 0x90;
			}
		}
	}
}
