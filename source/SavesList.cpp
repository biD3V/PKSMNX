#include "SavesList.hpp"
#include <fmt/format.h>
#include "SaveDetailView.hpp"

SavesList::SavesList(AccountUid uid) : AppletFrame(true,true)
{
    this->accUID = uid;
    this->setTitle("Saves");

    availableGames.clear();
    list = new brls::List();

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

    if (availableGames.size() > 0) {
        for (Game game : availableGames) {
            brls::ListItem *gameItem = new brls::ListItem(game.name,"",fmt::format("{:#x}",game.titleID));
            gameItem->getClickEvent()->subscribe([game,uid](brls::View *view){
                brls::Application::pushView(new SaveDetailView(uid,game));
            });
            list->addView(gameItem);
        }
    } else {
        noSaves = new brls::Label(brls::LabelStyle::REGULAR, "No matching saves. Did you select the correct user?");
        list->addView(noSaves);
    }
    this->setContentView(list);
}