#include "StdInclude.hpp"
#include "MemoryUtils.hpp"

namespace IWXMVM::MemoryUtils
{
    void UnprotectModule(std::uintptr_t module)
    {
        if (module == NULL)
            module = (std::uintptr_t)GetModuleHandle(NULL);

        PIMAGE_DOS_HEADER header = (PIMAGE_DOS_HEADER)module;
        PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)module + header->e_lfanew);

        SIZE_T size = ntHeader->OptionalHeader.SizeOfImage;
        DWORD oldProtect;
        VirtualProtect((LPVOID)module, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    }
}  // namespace IWXMVM::MemoryUtils