#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <switch.h>

#include <pkx/PKX.hpp>

struct Game {
    std::string name;
    u64 titleID;
};

struct BoxPKM {
    std::shared_ptr<pksm::PKX> pkm;
    u8 box;
    u8 slot;
};