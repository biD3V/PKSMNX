#include "profilesTab.hpp"

ProfilesTab::ProfilesTab()
{
    Result rc=0;
    AccountUid *uids = new AccountUid[ACC_USER_LIST_SIZE];
    s32 uidCount;
    accountListAllUsers(uids,ACC_USER_LIST_SIZE,&uidCount);

    for (size_t i = 0; i < uidCount; i++)
    {
        AccountProfile profile;
        AccountProfileBase base;
        std::string username;

        rc = accountGetProfile(&profile,uids[i]);

        if (R_SUCCEEDED(rc)) {
            rc = accountProfileGet(&profile,NULL,&base);
            if (R_SUCCEEDED(rc)){
                std::string userSafe;
                u128 uID128 = util::accountUIDToU128(uids[i]);
                username = base.nickname;
                userSafe = util::safeString(username);
                if(userSafe.empty())
                {
                    char tmp[32];
                    sprintf(tmp, "Acc%08x", (uint32_t)uID128);
                    userSafe = tmp;
                }
            }
        } else {
            username = "User ";
            username += std::to_string(i+1);
        }
        brls::ListItem *profileItem = new brls::ListItem(username);
        profileItem->getClickEvent()->subscribe([uids,i](brls::View* view) {
            brls::Application::pushView(new SavesList(uids[i]));
        });

        // profileItem->registerAction("Open Save",brls::Key::A, [this] {
        //     brls::Application::pushView(new SavesList(uids[i]));
        // }, true);


        this->addView(profileItem);
    }

    // Result romfsRC = romfsInit();
    // if (R_SUCCEEDED(romfsRC)) {
    //     this->addView(new brls::ListItemGroupSpacing(true));

    //     brls::List *romfsList = new brls::List();
    //     re
    // }
}