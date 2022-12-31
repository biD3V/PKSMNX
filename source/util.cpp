#include "util.hpp"

static const uint32_t verboten[] = { L',', L'/', L'\\', L'<', L'>', L':', L'"', L'|', L'?', L'*', L'™', L'©', L'®'};

static bool isVerboten(const uint32_t& t)
{
    for(unsigned i = 0; i < 13; i++)
    {
        if(t == verboten[i])
            return true;
    }

    return false;
}

uint32_t replaceChar(uint32_t c)
{
    switch(c)
    {
        case L'é':
            return 'e';
            break;
    }

    return c;
}

std::string util::safeString(const std::string& s)
{
    std::string ret = "";
    for(unsigned i = 0; i < s.length(); )
    {
        uint32_t tmpChr = 0;
        ssize_t untCnt = decode_utf8(&tmpChr, (uint8_t *)&s.data()[i]);

        i += untCnt;

        tmpChr = replaceChar(tmpChr);

        if(isVerboten(tmpChr))
            ret += ' ';
        else if(!isASCII(tmpChr))
            return ""; //return empty string so titledata::init defaults to titleID
        else
            ret += (char)tmpChr;
    }

    //Check for spaces at end
    while(ret[ret.length() - 1] == ' ' || ret[ret.length() - 1] == '.')
        ret.erase(ret.length() - 1, 1);

    return ret;
}

Result util::loadSave(Game game,AccountUid uid, std::shared_ptr<pksm::Sav> *save) {
    
    Result rc = 0;

    //You can use any device-name. If you want multiple saves mounted at the same time, you must use different device-names for each one.
    rc = fsdevMountSaveData("save", game.titleID, uid);//See also libnx fs.h/fs_dev.h
    if (R_FAILED(rc)) {
        printf("fsdevMountSaveData() failed: 0x%x\n", rc);
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

    return rc;
}

Result util::writeSave(Game game,AccountUid uid,std::shared_ptr<pksm::Sav> save) {
    Result rc = 0;

    rc = fsdevMountSaveData("save",game.titleID,uid);
    if (R_FAILED(rc)) {
        printf("fsdevMountSaveData() failed: 0x%x\n", rc);
        brls::Logger::error("fsdevMountSaveData() failed: {:#x}\n",rc);
    }

    if (R_SUCCEEDED(rc)) {
        FILE* saveFile = fopen("save:/main","wb");
        // u32 size;
        // std::shared_ptr<u8[]> saveData = nullptr;
        
        save->finishEditing();

        u32 writeRet;
        writeRet = fwrite(save->rawData().get(),1,save->getLength(),saveFile);
        fclose(saveFile);

        if (writeRet != save->getLength()) rc = 1;

        save->beginEditing();

        rc = fsdevCommitDevice("save");
        if (R_FAILED(rc)) {
            printf("fsdevCommitDevice() failed: 0x%x\n", rc);
            brls::Logger::error("fsdevCommitDevice() failed: {:#x}\n",rc);
        }
    }

    fsdevUnmountDevice("save");

    return rc;
}