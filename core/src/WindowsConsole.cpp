#include "StdInclude.hpp"
#include "WindowsConsole.hpp"

namespace IWXMVM
{
    FILE* stream = nullptr;

    void WindowsConsole::Open()
    {
        // Try to allocate the windows console, if it already exists we set the std handle to ours.
        if (GetConsoleWindow() == NULL)
        {
            AllocConsole();
            freopen_s(&stream, "CONOUT$", "w", stdout);
        }
        else
        {
            HANDLE hConsole = NULL;

            if (stream != NULL)
                hConsole = (HANDLE)_get_osfhandle(_fileno(stream));

            if (hConsole != NULL)
                SetStdHandle(STD_OUTPUT_HANDLE, hConsole);
        }
    }

    void WindowsConsole::Close()
    {
        fclose(stream);
        FreeConsole();
    }
}  // namespace IWXMVM