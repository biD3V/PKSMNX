#include "SavesList.hpp"

SavesList::SavesList(AccountUid uid) {
    this->accUID = uid;
    availableGames.clear();

    struct Game games[] = {
        {"Sword",0x0100ABF008968000},
        {"Shield",0x01008DB008C2C000},
        {"Scarlet",0x0100A3D008C5C000},
        {"Violet",0x01008F6008C5E000}
    };

    for (Game game : games) {
        if (R_SUCCEEDED(fsdevMountSaveData(game.name.c_str(),game.titleID,uid))) availableGames.push_back(game);
        fsdevUnmountDevice(game.name.c_str());
    }

    for (Game game : availableGames) {
        brls::ListItem *gameItem = new brls::ListItem(game.name,std::to_string(game.titleID));
        this->addView(gameItem);
    }
}