#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <borealis.hpp>
#include <switch.h>
#include "Game.h"
#include <sav/Sav.hpp>
#include "util.hpp"

#include "SaveDetailView.hpp"

class PKMInfoView : public brls::TabFrame
{
    public:
        bool pkmEdited;
        bool isParty;
        u8 box;
        u8 slot;
        std::unique_ptr<pksm::PKX> pkm;
        PKMInfoView(std::shared_ptr<pksm::Sav> &save, u8 box, u8 slot, bool party, bool& unsavedChanges);
        //bool onCancel() override;
        //void savePKM(std::shared_ptr<pksm::Sav> &save);
};