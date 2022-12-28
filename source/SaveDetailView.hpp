#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <borealis.hpp>
#include <switch.h>
#include "Game.h"
#include <sav/Sav.hpp>
#include "util.hpp"

class SaveDetailView : public brls::TabFrame
{
    public:
        std::shared_ptr<pksm::Sav> save;
        SaveDetailView(AccountUid uid, Game game);
};