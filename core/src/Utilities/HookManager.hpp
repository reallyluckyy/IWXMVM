#pragma once

namespace IWXMVM::HookManager
{
	void WriteJump(std::uintptr_t from, const std::uintptr_t to, std::size_t len = 5);

	// TODO: Store placed hooks and unhook in Unhook function

	template<typename FunctionSignature>
	FunctionSignature CreateHook(uintptr_t functionAddress, FunctionSignature hook, uint8_t len)
	{
		const auto trampolineSize = len + 5;

		auto trampolineAddress = reinterpret_cast<std::uintptr_t>(VirtualAlloc(
			nullptr,
			trampolineSize,
			MEM_COMMIT | MEM_RESERVE,
			PAGE_EXECUTE_READWRITE));

		std::memcpy(reinterpret_cast<void*>(trampolineAddress), reinterpret_cast<const void*>(functionAddress), len);

		trampolineAddress += len;

		WriteJump(trampolineAddress, functionAddress + len);

		DWORD oldProtection;
		VirtualProtect(reinterpret_cast<void*>(trampolineAddress), trampolineSize, PAGE_EXECUTE_READ, &oldProtection);

		WriteJump(functionAddress, (uintptr_t)hook, len);

		return (FunctionSignature)(trampolineAddress - len);
	}
};
