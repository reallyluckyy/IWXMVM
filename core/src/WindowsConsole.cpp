#include "StdInclude.hpp"
#include "WindowsConsole.hpp"

namespace IWXMVM
{
void WindowsConsole::Open()
{
    if (AllocConsole())
    {
        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w", stdout);
    }
}

void WindowsConsole::Close()
{
    FreeConsole();
}
}  // namespace IWXMVM