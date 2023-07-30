#include "StdInclude.hpp"
#include "Mod.hpp"

#include "IW3Interface.hpp"

using namespace IWXMVM;
using namespace IW3;

IW3Interface gameInterface = IW3Interface();

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) 
{
    if (fdwReason == DLL_PROCESS_ATTACH) 
    {
        std::thread t1{ []() { Mod::Initialize(&gameInterface); } };
        t1.detach();
    }
    return TRUE;
}