#include "SaveDetailView.hpp"

SaveDetailView::SaveDetailView(AccountUid uid, Game game) : TabFrame()
{
    Result rc = 0;
    this->partyPKM.clear();
    this->hasChanges = false;
    this->uid = uid;
    this->game = game;

    this->setTitle(game.name);

    rc = util::loadSave(game,uid,&this->save);
    
    if (R_SUCCEEDED(rc)) {
        
        brls::List *trainerInfo = new brls::List();
        trainerInfo->addView(new brls::InputListItem("Name",this->save->otName(),"Input trainer name, max characters 12","",12));
        this->addTab("Trainer",trainerInfo);

        this->addSeparator();

        brls::List *party = new brls::List();
        for (u8 i = 0; i < this->save->partyCount(); i++)
        {
            brls::ListItem *pkmItem = new brls::ListItem(this->save->pkm(i)->nickname());
            pkmItem->getClickEvent()->subscribe([this,i](brls::View *view){
                PKMInfoView *pkmInfo = new PKMInfoView(this->save,0,i,true,this->hasChanges);
                brls::Application::pushView(pkmInfo);
            });
            party->addView(pkmItem);
        }
        this->addTab("Party",party);

        brls::List *boxes = new brls::List();
        for (u8 b = 0; b < this->save->unlockedBoxes(); b++)
        {
            std::string boxName = this->save->boxName(b) != "" ? this->save->boxName(b) : fmt::format("Box {:d}",b+1);
            brls::ListItem *boxItem = new brls::ListItem(boxName);
            boxItem->getClickEvent()->subscribe([this,b,boxName](brls::View *view){
                brls::List *boxList = new brls::List();
                for (u8 p = 0; p < 30; p++)
                {
                    if (this->save->pkm(b,p).get() != NULL) {
                        brls::ListItem *pkmItem = new brls::ListItem(this->save->pkm(b,p)->nickname());
                        pkmItem->getClickEvent()->subscribe([this,b,p](brls::View *view){
                            PKMInfoView *pkmInfo = new PKMInfoView(this->save,b,p,false,this->hasChanges);
                            brls::Application::pushView(pkmInfo);
                        });
                        boxList->addView(pkmItem);
                    }
                }
                brls::AppletFrame *boxFrame = new brls::AppletFrame(true,true);
                boxFrame->setTitle(boxName);
                boxFrame->setContentView(boxList);
                brls::Application::pushView(boxFrame);
            });
            boxes->addView(boxItem);
        }
        this->addTab("Boxes",boxes);
        

        this->registerAction("Save", brls::Key::X, [this]() {
            Result rc = util::writeSave(this->game,this->uid,this->save);
            if (R_FAILED(rc)) {
                brls::Application::notify("Save Failed!");
            } else {
                brls::Application::notify("Saved.");
                this->hasChanges = false;
            }
            return true;
        });
    }
}

bool SaveDetailView::onCancel() {
    if (this->hasChanges)
    {
  
        Result rc = 0;
        brls::Dialog *dialog = new brls::Dialog("Would you like to save your changes?");
        
        dialog->addButton("Yes",[this,dialog,&rc](brls::View *v){
            rc = util::writeSave(this->game,this->uid,this->save);
            if (R_FAILED(rc)) {
                dialog->close();
                brls::Application::notify("Save Failed!");
            } else {
                dialog->close([](){
                    brls::Application::popView();
                });
            }
        });

        dialog->addButton("No",[dialog](brls::View *v){
            dialog->close([](){
                brls::Application::popView();
            });
        });

        dialog->open();
        
        // if (R_FAILED(rc)) {
        //     brls::Dialog *saveFailed = new brls::Dialog(fmt::format("Saving Failed. Close anyway?\n{:#x}",rc));

        //     saveFailed->addButton("Yes",[saveFailed](brls::View *v){
        //         saveFailed->close([](){
        //             brls::Application::popView();
        //         });
        //     });

        //     saveFailed->addButton("No",[saveFailed](brls::View *v){
        //         saveFailed->close();
        //     });

        //     saveFailed->open();
        // }
    } else {
        brls::Application::popView();
    }
    return true;
}