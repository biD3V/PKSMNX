#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <switch.h>

namespace util
{
    inline bool isASCII(const uint32_t& t)
    {
        return t > 30 && t < 127;
    }

    inline u128 accountUIDToU128(AccountUid uid)
    {
        return ((u128)uid.uid[0] << 64 | uid.uid[1]);
    }

    std::string safeString(const std::string& s);
} // namespace util