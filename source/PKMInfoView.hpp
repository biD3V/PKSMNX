#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <borealis.hpp>
#include <switch.h>
#include "Game.h"
#include <sav/Sav.hpp>
#include <pkx/PK9.hpp>

#include <vector>
#include <string>

#include "util.hpp"

#include "SaveDetailView.hpp"

class PKMInfoView : public brls::TabFrame
{
    private:
        u8 getAbilityListNumber(u8 n);
        
    public:
        bool pkmEdited;
        bool isParty;
        u8 box;
        u8 slot;
        std::unique_ptr<pksm::PKX> pkm;
        PKMInfoView(std::shared_ptr<pksm::Sav> &save, u8 box, u8 slot, bool party, bool& unsavedChanges);
        void addChangesToSave(std::shared_ptr<pksm::Sav> &save);
};