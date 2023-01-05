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

    rc = backupSave(uid,game);
    if (R_SUCCEEDED(rc)) {
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
    }

    return rc;
}

Result util::backupSave(AccountUid uid,Game game) {
    Result rc = 0;

    std::string backupDir = "sdmc:/PKSMNX";
    struct stat backupStat;
    if (stat(backupDir.c_str(), &backupStat) != 0) {
        if(mkdir(backupDir.c_str(),S_IRWXU|S_IRWXG|S_IROTH) == -1) {
            return 1;
        }
    }

    AccountProfile profile;
    AccountProfileBase base;
    accountGetProfile(&profile,uid);
    accountProfileGet(&profile,NULL,&base);
    std::string accountDir = backupDir + "/" + safeString(base.nickname);
    struct stat accountStat;
    if (stat(accountDir.c_str(), &accountStat) != 0) {
        //rc = mkdir(accountDir,S_IRWXU|S_IRWXG|S_IROTH);
        if (mkdir(accountDir.c_str(),S_IRWXU|S_IRWXG|S_IROTH) == -1) {
            return 1;
        }
    }

    std::string gameDir = accountDir + "/" + game.name;
    struct stat gameStat;
    if (stat(gameDir.c_str(), &gameStat) != 0) {
        rc = mkdir(gameDir.c_str(),S_IRWXU|S_IRWXG|S_IROTH);
        if (R_FAILED(rc)) return rc;
    }

    //const char* backupFile = fmt::format("{}/main",gameDir).c_str();
    
    rc = fsdevMountSaveData("save",game.titleID,uid);
    if (R_FAILED(rc)) {
        printf("fsdevMountSaveData() failed: 0x%x\n", rc);
        brls::Logger::error("fsdevMountSaveData() failed: {:#x}\n",rc);
        return rc;
    }

    if (R_SUCCEEDED(rc)) {
        // FILE* sourceSave = fopen("save:/main","rb");
        // if (sourceSave == NULL) {
        //     return rc;
        // }

        // FILE* backupSave = fopen(backupFile, "wb");
        // if (backupSave == NULL) {
        //     return rc;
        // }

        // char c = fgetc(sourceSave);
        // while (c != EOF) {
        //     fputc(c, backupSave);
        //     c = fgetc(sourceSave);
        // }

        // fclose(sourceSave);
        // fclose(backupSave);
        rc = copyDir("save:/",gameDir);
    }

    fsdevUnmountDevice("save");

    return rc;
}

Result util::copyDir(std::string src, std::string dest) {
    Result rc = 0;

    DIR* dir;
    struct dirent* ent;

    std::vector<std::string> items;

    dir = opendir(src.c_str());
    if (!dir) {
        return 1;
    } else {
        while ((ent = readdir(dir)))
        {
            items.emplace_back(ent->d_name);
        }
        closedir(dir);
        
        for (size_t i = 0; i < items.size(); i++)
        {
            std::string srcPath = src + "/" + items[i];
            std::string destPath = dest + "/" + items[i];
            if (isDir(src + "/" + items[i])) {

                mkdir(destPath.c_str(),S_IRWXU|S_IRWXG|S_IROTH);
                copyDir(srcPath,destPath);
            } else {
                FILE* srcFile = fopen(srcPath.c_str(), "rb");
                if (!srcFile) return 1;
                
                FILE* destFile = fopen(destPath.c_str(), "wb");
                if (!destFile) return 1;

                char c = fgetc(srcFile);
                while (c != EOF) {
                    fputc(c, destFile);
                    c = fgetc(srcFile);
                }

                fclose(srcFile);
                fclose(destFile);
            }
        }
        
    }
    return rc;
}

bool util::isDir(std::string dir) {
    struct stat s;
    if (stat(dir.c_str(),&s) == 0 && S_ISDIR(s.st_mode)) {
        return true;
    }
    return false;
}