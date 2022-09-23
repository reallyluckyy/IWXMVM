#pragma once

namespace IWXMVM::HookManager
{
	// TODO: Store placed hooks and unhook in Unhook function

	bool WriteJump(std::uintptr_t from, std::uintptr_t to, std::size_t size);
	void* CreateHook(std::uintptr_t from, std::uintptr_t to, std::size_t size, bool stolenBytes);
};
