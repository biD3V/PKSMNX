#pragma once

#include <borealis.hpp>
#include <switch.h>
#include "Game.h"

class SavesList : public brls::AppletFrame
{
private:
    AccountUid accUID;
    std::vector<Game> availableGames;
    brls::List *list;
    brls::Label *noSaves;

public:
    SavesList(AccountUid uid);
};
