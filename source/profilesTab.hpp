#pragma once

#include <borealis.hpp>
#include <switch.h>
#include "util.hpp"
#include "SavesList.hpp"

class ProfilesTab : public brls::List
{
    private:
        brls::ListItem *testItem;
        
    public:
        ProfilesTab();
};