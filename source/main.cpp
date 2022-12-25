// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

// Include the main libnx system header, for Switch development
#include <switch.h>


#include <dirent.h>
#include <iostream>
#include <fstream>

#include <sav/Sav.hpp>
#include <pkx/PKX.hpp>
#include <pkx/PK9.hpp>
#include <utils/i18n.hpp>

struct Game {
    std::string name;
    u64 titleID;
};

enum PAGE {
    PAGE_ACCOUNTS,
    PAGE_SAVES,
    PAGE_DETAILS,
};

void loadSaves(AccountUid uid, std::vector<Game> *availableGames) {
    consoleClear();
    availableGames->clear();

    struct Game games[] = {
        {"Sword",0x0100ABF008968000},
        {"Shield",0x01008DB008C2C000},
        {"Scarlet",0x0100A3D008C5C000},
        {"Violet",0x01008F6008C5E000}
    };

    for (Game game : games) {
        if (R_SUCCEEDED(fsdevMountSaveData(game.name.c_str(),game.titleID,uid))) availableGames->push_back(game);
        fsdevUnmountDevice(game.name.c_str());
    }
}

void saveSelection(std::vector<Game> games, int selection, AccountUid uid) {
    consoleClear();

    selection = selection % games.size();

    for (size_t i = 0; i < games.size(); i++)
    {
        if (i == selection) printf("> ");
        else printf("  ");
        printf("%s\n",games[i].name.c_str());
    }
}

void loadSave(Game game,AccountUid uid, std::shared_ptr<pksm::Sav> *save) {
    
    Result rc = 0;

    //You can use any device-name. If you want multiple saves mounted at the same time, you must use different device-names for each one.
    if (R_SUCCEEDED(rc)) {
        rc = fsdevMountSaveData("save", game.titleID, uid);//See also libnx fs.h/fs_dev.h
        if (R_FAILED(rc)) {
            printf("fsdevMountSaveData() failed: 0x%x\n", rc);
        }
    }

    //At this point you can use the mounted device with standard stdio.
    //After modifying savedata, in order for the changes to take affect you must use: rc = fsdevCommitDevice("save");
    //See also libnx fs_dev.h for fsdevCommitDevice.

    if (R_SUCCEEDED(rc)) {
        FILE* saveFile = fopen("save:/main","rb");
        u32 size;
        std::shared_ptr<u8[]> saveData = nullptr;

        if (saveFile) {
            fseek(saveFile,0,SEEK_END);
            size = ftell(saveFile);
            rewind(saveFile);

            if (size > 0x319DC3) {
                printf("Save too big: 0x%x\n", size);
                fclose(saveFile);
            } else {
                saveData = std::shared_ptr<u8[]>(new u8[size]);
                fread(saveData.get(), 1, size, saveFile);
                fclose(saveFile);

                *save = pksm::Sav::getSave(saveData, size);

                if (save) {
                    printf("save loaded");
                } else {
                    printf("save wrong");
                }
            }
        } else {
            printf("Could not open file");
        }

        //When you are done with savedata, you can use the below.
        //Any devices still mounted at app exit are automatically unmounted.
        fsdevUnmountDevice("save");
    }
}

void saveOverview(std::shared_ptr<pksm::Sav> *save, int selection) {
    consoleClear();

    selection = selection % 2;

    if (save) {
        printf("Trainer name: %s\n", save->get()->otName().c_str());
        printf("Play Time: %i:%i:%i\n", save->get()->playedHours(), save->get()->playedMinutes(), save->get()->playedSeconds());
        printf("Money: %i\n",save->get()->money());
        printf("BP: %i\n",save->get()->BP());
        if (selection == 0) printf("> Party\n  Boxes");
        else printf("  Party\n> Boxes");
        
        // printf("Party:\n");
        // for (size_t i = 0; i < save->partyCount(); i++)
        // {
        //     std::shared_ptr<pksm::PKX> pkm = save.get()->pkm(i);
        //     printf("    %s\n", pkm->nickname().c_str());
        //     printf("        EVs:\n");
        //     printf("            HP: %i  ATK: %i  SPATK: %i  DEF: %i  SPDEF: %i  SPD: %i\n",pkm->ev(pksm::Stat::HP),pkm->ev(pksm::Stat::ATK),pkm->ev(pksm::Stat::SPATK),pkm->ev(pksm::Stat::DEF),pkm->ev(pksm::Stat::SPDEF),pkm->ev(pksm::Stat::SPD));
        // }
    }
}

void showAccountSelection(AccountUid *uids, s32 total, int selection) {
    consoleClear();

    selection = selection % total;

    for (size_t i = 0; i < total; i++)
    {
        if (selection == i) printf("> ");
        else printf("  ");
        printf("User %li    0x%lx 0x%lx\n",i + 1,uids[i].uid[0],uids[i].uid[1]);
    }
    
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    // This example uses a text console, as a simple way to output text to the screen.
    // If you want to write a software-rendered graphics application,
    //   take a look at the graphics/simplegfx example, which uses the libnx Framebuffer API instead.
    // If on the other hand you want to write an OpenGL based application,
    //   take a look at the graphics/opengl set of examples, which uses EGL instead.
    consoleInit(NULL);

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    int selection = 0;
    int saveSelec = 0;
    int boxSelect = 0;

    accountInitialize(AccountServiceType_Administrator);
    AccountUid *uids = new AccountUid[ACC_USER_LIST_SIZE];
    s32 uidCount;
    accountListAllUsers(uids,ACC_USER_LIST_SIZE,&uidCount);
    
    enum PAGE page = PAGE_ACCOUNTS;
    showAccountSelection(uids,uidCount,selection);

    std::vector<Game> availableGames;
    std::shared_ptr<pksm::Sav> save;

    // Main loop
    while (appletMainLoop())
    {
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        // padGetButtonsDown returns the set of buttons that have been
        // newly pressed in this frame compared to the previous one
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus)
            break; // break in order to return to hbmenu

        if (kDown & HidNpadButton_AnyDown) {
            switch (page)
            {
            case PAGE_ACCOUNTS:
                selection++;
                showAccountSelection(uids,uidCount,selection);
                break;
            
            case PAGE_SAVES:
                saveSelec++;
                saveSelection(availableGames,saveSelec,uids[selection]);
                break;

            case PAGE_DETAILS:
                boxSelect++;
                saveOverview(&save,boxSelect);
                break;

            default:
                break;
            }
        }

        if (kDown & HidNpadButton_AnyUp) {
            switch (page)
            {
            case PAGE_ACCOUNTS:
                selection+=-1;
                showAccountSelection(uids,uidCount,selection);
                break;
            
            case PAGE_SAVES:
                saveSelec+=-1;
                saveSelection(availableGames,saveSelec,uids[selection]);
            
            case PAGE_DETAILS:
                boxSelect+=-1;
                saveOverview(&save,boxSelect);
                break;

            default:
                break;
            }
        }

        if (kDown & HidNpadButton_A) {
            switch (page)
            {
            case PAGE_ACCOUNTS:
                page = PAGE_SAVES;
                loadSaves(uids[selection],&availableGames);
                saveSelection(availableGames,saveSelec,uids[selection]);
                break;
            
            case PAGE_SAVES:
                page = PAGE_DETAILS;
                loadSave(availableGames[saveSelec],uids[selection],&save);
                saveOverview(&save,boxSelect);
                break;

            default:
                showAccountSelection(uids,uidCount,selection);
                break;
            }
        }

        if (kDown & HidNpadButton_B) {
            page = PAGE_ACCOUNTS;
            showAccountSelection(uids,uidCount,selection);
        }

        // Your code goes here

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }

    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}