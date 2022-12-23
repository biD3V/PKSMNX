// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>


#include <dirent.h>
#include <iostream>
#include <fstream>

#include <sav/Sav.hpp>
#include <pkx/PKX.hpp>
#include <pkx/PK9.hpp>

// Main program entrypoint
int main(int argc, char* argv[])
{
    Result rc = 0;

    DIR* dir;
    struct dirent* ent;

    AccountUid uid={0};
    u64 application_id=0x01008F6008C5E000;//ApplicationId of the save to mount, in this case BOTW.

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

    rc = accountInitialize(AccountServiceType_Application);
    if (R_FAILED(rc)) {
        printf("accountInitialize() failed: 0x%x\n", rc);
    }

    if (R_SUCCEEDED(rc)) {
        rc = accountGetPreselectedUser(&uid);
        accountExit();

        if (R_FAILED(rc)) {
            printf("accountGetPreselectedUser() failed: 0x%x\n", rc);
        }
    }

    if (R_SUCCEEDED(rc)) {
        printf("Using application_id=0x%016lx\n", application_id);
    }

    //You can use any device-name. If you want multiple saves mounted at the same time, you must use different device-names for each one.
    if (R_SUCCEEDED(rc)) {
        rc = fsdevMountSaveData("save", application_id, uid);//See also libnx fs.h/fs_dev.h
        if (R_FAILED(rc)) {
            printf("fsdevMountSaveData() failed: 0x%x\n", rc);
        }
    }

    //At this point you can use the mounted device with standard stdio.
    //After modifying savedata, in order for the changes to take affect you must use: rc = fsdevCommitDevice("save");
    //See also libnx fs_dev.h for fsdevCommitDevice.
    
    std::shared_ptr<pksm::Sav> save;

    if (R_SUCCEEDED(rc)) {
        dir = opendir("save:/");//Open the "save:/" directory.
        if(dir==NULL)
        {
            printf("Failed to open dir.\n");
        }
        else
        {
            printf("Dir-listing for 'save:/':\n");
            while ((ent = readdir(dir)))
            {
                printf("d_name: %s\n", ent->d_name);
            }
            closedir(dir);
            printf("Done.\n");
        }

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

                save = pksm::Sav::getSave(saveData, size);

                if (save) {
                    printf("save loaded");
                    // printf("Trainer name: %s", save.get()->otName().c_str());
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

        // Your code goes here
        if (kDown & HidNpadButton_A && save) {
            printf("\n\nTrainer name: %s\n", save.get()->otName().c_str());
            printf("Play Time: %i:%i:%i\n", save.get()->playedHours(), save.get()->playedMinutes(), save.get()->playedSeconds());
            printf("Party:\n");
            for (size_t i = 0; i < save->partyCount(); i++)
            {
                std::shared_ptr<pksm::PKX> pkm = save.get()->pkm(i);
                printf("    %s\n", pkm->nickname().c_str());
                printf("        %i/%i/%i/%i/%i/%i\n",pkm->ev(pksm::Stat::HP),pkm->ev(pksm::Stat::ATK),pkm->ev(pksm::Stat::SPATK),pkm->ev(pksm::Stat::DEF),pkm->ev(pksm::Stat::SPDEF),pkm->ev(pksm::Stat::SPD));
            }
            
            // if (save.get()->pkm(1)) {
            //     std::shared_ptr<pksm::PKX> pkm = save.get()->pkm(0);
            //     printf("    %s\n", pkm->nickname().c_str());
            // }
            // if (save.get()->pkm(2)) {
            //     std::shared_ptr<pksm::PKX> pkm = save.get()->pkm(0);
            //     printf("    %s\n", pkm->nickname().c_str());
            // }
        }

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }

    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}