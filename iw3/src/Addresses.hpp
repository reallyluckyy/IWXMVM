#pragma once
#include "StdInclude.hpp"
#include "Signatures.hpp"

namespace IWXMVM::IW3
{
    using IW3Addresses = Signatures::IW3Addresses;

    inline const IW3Addresses& GetGameAddresses()
    {
        static IW3Addresses addresses;
        return addresses;
    }
}  // namespace IWXMVM::IW3