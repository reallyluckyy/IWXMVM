#include "StdInclude.hpp"
#include "WindowsConsole.hpp"

namespace IWXMVM
{
    FILE* stream = nullptr;

    void WindowsConsole::Open()
    {
        if (AllocConsole())
        {
            freopen_s(&stream, "CONOUT$", "w", stdout);
        }
    }

    void WindowsConsole::Close()
    {
        fclose(stream);
        FreeConsole();
    }
}  // namespace IWXMVM