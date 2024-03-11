#include "StdInclude.hpp"
#include "Structures.hpp"

#include "Addresses.hpp"

// TODO: We might have to split this up into several files

namespace IWXMVM::IW3::Structures
{
    clientConnection_t* GetClientConnection()
    {
        return (clientConnection_t*)GetGameAddresses().clientConnection();
    }

    clientStatic_t* GetClientStatic()
    {
        return (clientStatic_t*)GetGameAddresses().clientStatic();
    }

    clientActive_t* GetClientActive()
    {
        return (clientActive_t*)GetGameAddresses().clientActive();
    }

    cgs_t* GetClientGlobalsStatic()
    {
        return (cgs_t*)GetGameAddresses().clientGlobalsStatic();
    }

    cg_s* GetClientGlobals()
    {
        return (cg_s*)GetGameAddresses().clientGlobals();
    }

    WinMouseVars_t* GetMouseVars()
    {
        return (WinMouseVars_t*)GetGameAddresses().mouseVars();
    }

    clientUIActive_t* GetClientUIActives()
    {
        return (clientUIActive_t*)GetGameAddresses().clientUIActives();
    }

    centity_s* GetEntities()
    {
        return (centity_s*)GetGameAddresses().cg_entities();
    }

    uint16_t* GetClientObjectMap()
    {
        return (uint16_t*)GetGameAddresses().clientObjMap();
    }

    DObj_s* GetObjBuf()
    {
        return (DObj_s*)GetGameAddresses().objBuf();
    }

}  // namespace IWXMVM::IW3::Structures
