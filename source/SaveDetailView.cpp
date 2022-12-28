#include "SaveDetailView.hpp"

SaveDetailView::SaveDetailView(AccountUid uid, Game game) : TabFrame()
{
    Result rc = 0;

    rc = util::loadSave(game,uid,&save);
    if (R_SUCCEEDED(rc)) {
        brls::List *trainerInfo = new brls::List();
        trainerInfo->addView(new brls::Label(brls::LabelStyle::MEDIUM,save->otName()));
        this->addTab("Trainer",trainerInfo);
    }
}