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
        // {"Let's Go, Pikachu!",0x010003F003A34000}, // Crashes
        // {"Let's Go, Eevee!",0x0100187003A36000},
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
                SaveDetailView *detailView = new SaveDetailView(uid,game);
                detailView->registerAction("Save", brls::Key::MINUS, [detailView]() {
                    Result rc = util::writeSave(detailView->game,detailView->uid,detailView->save);
                    if (R_FAILED(rc)) {
                        brls::Application::notify("Save Failed!");
                    } else {
                        brls::Application::notify("Saved.");
                        detailView->hasChanges = false;
                    }
                    return true;
                });
                brls::Application::pushView(detailView);
            });
            list->addView(gameItem);
        }
    } else {
        noSaves = new brls::Label(brls::LabelStyle::REGULAR, "No matching saves. Did you select the correct user?");
        list->addView(noSaves);
    }
    this->setContentView(list);
}