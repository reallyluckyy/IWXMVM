#include "StdInclude.hpp"
#include "Structures.hpp"

#include "Addresses.hpp"

namespace IWXMVM::IW5::Structures
{
    cg_s* GetClientGlobals()
    {
        return reinterpret_cast<cg_s*>(0x900FA0);
    }

    clientStatic_t* GetClientStatic()
    {
        return reinterpret_cast<clientStatic_t*>(0xBA4DC8);
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
