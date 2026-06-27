#pragma once
#include "StdInclude.hpp"
#include "Signatures.hpp"

namespace IWXMVM::IW5
{
    using IW5Addresses = Signatures::IW5Addresses;

    inline const IW5Addresses& GetGameAddresses()
    {
        static IW5Addresses addresses;
        return addresses;
    }
}  // namespace IWXMVM::IW3