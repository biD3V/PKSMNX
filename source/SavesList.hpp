#pragma once

#include <borealis.hpp>
#include <switch.h>

class SavesList : public brls::List
{
private:
    struct Game {
        std::string name;
        u64 titleID;
    };
    AccountUid accUID;
    std::vector<Game> availableGames;
public:
    SavesList(AccountUid uid);
};
