#include "StdInclude.hpp"
#include "Structures.hpp"

#include "Addresses.hpp"

namespace IWXMVM::IW5::Structures
{
    cg_s* GetClientGlobals()
    {
        return reinterpret_cast<cg_s*>(0x900FA0);
    }

    cgs_t* GetClientGlobalsStatic()
    {
        return reinterpret_cast<cgs_t*>(0x8FCA20);
    }

    clientActive_t* GetClientActive()
    {
        return reinterpret_cast<clientActive_t*>(0x1062790);
    }

    centity_s* GetEntities()
    {
        return reinterpret_cast<centity_s*>(0xA0A5C0);
    }

    clientInfo_t* GetClientInfo()
    {
        return reinterpret_cast<clientInfo_t*>(0x9FE678);
    }

    clientStatic_t* GetClientStatic()
    {
        return reinterpret_cast<clientStatic_t*>(0xBA4DC8);
    }

    gameState_t* GetGameState()
    {
        return reinterpret_cast<gameState_t*>(0xFF7434);
    }

    clientConnection_t* GetClientConnection()
    {
        return *reinterpret_cast<clientConnection_t**>(0x10625F4);
    }

    clientDemoPlayback_t* GetClientDemoPlayback()
    {
        return *reinterpret_cast<clientDemoPlayback_t**>(0x66148C0);
    }

    demoCameraData_t* GetDemoCameraData()
    {
        return reinterpret_cast<demoCameraData_t*>(0x970D60);
    }
}  // namespace IWXMVM::IW5::Structures
