#include "StdInclude.hpp"
#include "Mod.hpp"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) 
{
    using namespace IWXMVM;

    if (fdwReason == DLL_PROCESS_ATTACH) {
        Mod::Initialize(Game::IW3);
    }
    return TRUE;
}
