#pragma once
#include "StdInclude.hpp"
#include "Signatures.hpp"

namespace IWXMVM::T4
{
    using T4Addresses = Signatures::T4Addresses;

    inline const T4Addresses& GetGameAddresses()
    {
        static T4Addresses addresses;
        return addresses;
    }
}  // namespace IWXMVM::T4