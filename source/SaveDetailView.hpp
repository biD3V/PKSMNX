#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include <borealis.hpp>
#include <switch.h>
#include "Game.h"
#include <sav/Sav.hpp>
#include "util.hpp"
#include "PKMInfoView.hpp"

#include <borealis/grid.hpp>

class SaveDetailView : public brls::TabFrame
{
    private:
        std::vector<BoxPKM> partyPKM;

    public:
        std::shared_ptr<pksm::Sav> save;
        AccountUid uid;
        Game game;
        bool hasChanges;
        SaveDetailView(AccountUid uid, Game game);
        bool onCancel() override;
};