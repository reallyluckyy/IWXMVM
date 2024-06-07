#include "StdInclude.hpp"
#include "Structures.hpp"

#include "Addresses.hpp"

namespace IWXMVM::IW5::Structures
{
    clientConnection_t* GetClientConnection()
    {
        return *reinterpret_cast<clientConnection_t**>(0x10625F4);
    }

    clientDemoPlayback_t* GetClientDemoPlayback()
    {
        return *reinterpret_cast<clientDemoPlayback_t**>(0x66148C0);
    }
}  // namespace IWXMVM::IW5::Structures
