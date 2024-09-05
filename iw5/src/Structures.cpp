#include "StdInclude.hpp"
#include "Structures.hpp"

#include "Addresses.hpp"

namespace IWXMVM::IW5::Structures
{
    cg_s* GetClientGlobals()
    {
        return reinterpret_cast<cg_s*>(GetGameAddresses().clientGlobals());
    }

    cgs_t* GetClientGlobalsStatic()
    {
        return reinterpret_cast<cgs_t*>(GetGameAddresses().clientGlobalsStatic());
    }

    clientActive_t* GetClientActive()
    {
        return reinterpret_cast<clientActive_t*>(GetGameAddresses().clientActive());
    }

    centity_s* GetEntities()
    {
        return reinterpret_cast<centity_s*>(GetGameAddresses().cg_entities());
    }

    clientInfo_t* GetClientInfo()
    {
        return reinterpret_cast<clientInfo_t*>(GetGameAddresses().clientInfo());
    }

    clientStatic_t* GetClientStatic()
    {
        return reinterpret_cast<clientStatic_t*>(GetGameAddresses().clientStatic());
    }

    gameState_t* GetGameState()
    {
        return reinterpret_cast<gameState_t*>(GetGameAddresses().cls_gameState());
    }

    clientConnection_t* GetClientConnection()
    {
        return reinterpret_cast<clientConnection_t*>(GetGameAddresses().clientConnection());
    }

    clientDemoPlayback_t* GetClientDemoPlayback()
    {
        return *reinterpret_cast<clientDemoPlayback_t**>(GetGameAddresses().s_clientDemoPlayback());
    }

    sun* GetSun()
    {
        return reinterpret_cast<sun*>(0x96C370); // TODO: Use sig
    }
}  // namespace IWXMVM::IW5::Structures
