#include "PKMInfoView.hpp"

PKMInfoView::PKMInfoView(std::shared_ptr<pksm::Sav> &save, u8 box, u8 slot, bool party, bool& unsavedChanges) : TabFrame()
{
    if (party) {
        this->pkm = save->pkm(slot);
    } else {
        this->pkm = save->pkm(box,slot);
    }
    brls::Logger::debug("Loaded info for {:s}",this->pkm->nickname());
    this->box = box;
    this->slot = slot;
    this->isParty = party;
    this->setTitle(this->pkm->nickname());
    this->pkmEdited = false;
    // this->pkm = pkm;

    brls::List *evList = new brls::List();
    // evList->addView(new brls::IntegerInputListItem("HP",pkm->ev(pksm::Stat::HP),"","",3));
    // evList->addView(new brls::IntegerInputListItem("Attack",pkm->ev(pksm::Stat::ATK),"","",3));
    // evList->addView(new brls::IntegerInputListItem("Special Attack",pkm->ev(pksm::Stat::SPATK),"","",3));
    // evList->addView(new brls::IntegerInputListItem("Deffence",pkm->ev(pksm::Stat::DEF),"","",3));
    // evList->addView(new brls::IntegerInputListItem("Special Deffence",pkm->ev(pksm::Stat::SPDEF),"","",3));
    // evList->addView(new brls::IntegerInputListItem("Speed",pkm->ev(pksm::Stat::SPD),"","",3));

    for (u8 i = 0; i < 6; i++)
    {
        std::string statName;
        pksm::Stat stat = static_cast<pksm::Stat>(i);
        switch (stat)
        {
        case pksm::Stat::HP:
            statName = "HP";
            break;

        case pksm::Stat::ATK:
            statName = "ATK";
            break;
        
        case pksm::Stat::SPATK:
            statName = "SPATK";
            break;

        case pksm::Stat::DEF:
            statName = "DEF";
            break;

        case pksm::Stat::SPDEF:
            statName = "SPDEF";
            break;

        case pksm::Stat::SPD:
            statName = "SPD";
            break;
        
        default:
            statName = "INVALID";
            break;
        }

        brls::IntegerInputListItem *evInputItem = new brls::IntegerInputListItem(statName,this->pkm->ev(stat),"","",3);
        evInputItem->getClickEvent()->subscribe([this,evInputItem,stat,&save,&unsavedChanges](brls::View *v){
            u16 value = std::stoi(evInputItem->getValue());

            if (value > 252) {
                brls::Application::notify("\uE5CD An EV has a max value of 252.");
                evInputItem->setValue(std::to_string(this->pkm->ev(stat)));
            } else if (value < 0) {
                brls::Application::notify("\uE5CD An EV cannot be negative.");
                evInputItem->setValue(std::to_string(this->pkm->ev(stat)));
            } else {
                this->pkm->ev(stat,value);
                // SaveDetailView *parentView = this->parent;
                if (this->isParty) {
                    save->pkm(*this->pkm,this->slot);
                } else {
                    save->pkm(*this->pkm,this->box,this->slot,false);
                }
                //save->fixParty();
                this->pkmEdited = true;
                unsavedChanges = true;
            }
        });

        evList->addView(evInputItem);
    }
    

    this->addTab("EVs",evList);
}

// bool PKMInfoView::onCancel() {
//     if (this->pkmEdited)
//     {
//         brls::Dialog *dialog = new brls::Dialog("Would you like to save your changes?");
        
//         dialog->addButton("Yes",[this,dialog](brls::View *v){
            
            
//             // util::writeSave(this->game,this->uid,this->save);

//             dialog->close([](){
//                 brls::Application::popView();
//             });
//         });

//         dialog->addButton("No",[dialog](brls::View *v){
//             dialog->close([](){
//                 brls::Application::popView();
//             });
//         });

//         dialog->open();
//     } else {
//         brls::Application::popView();
//     }
//     return true;
// }

// void PKMInfoView::savePKM(std::shared_ptr<pksm::Sav> &save) {
//     if (this->isParty) {
//         save->pkm(this->pkm,slot);
//     } else {
//         save->pkm(this->pkm,box,slot,false);
//     }
// }