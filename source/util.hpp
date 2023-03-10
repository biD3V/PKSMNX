#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <switch.h>

#include <sys/stat.h>
#include <dirent.h>

#include "Game.h"

#include <sav/Sav.hpp>

#include <borealis.hpp>

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

    Result loadSave(Game game,AccountUid uid, std::shared_ptr<pksm::Sav> *save);
    Result writeSave(Game game,AccountUid uid, std::shared_ptr<pksm::Sav> save);
    Result backupSave(AccountUid uid,Game game);
    Result copyDir(std::string src, std::string dest);
    bool isDir(std::string dir);
} // namespace util