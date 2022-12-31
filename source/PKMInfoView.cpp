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

    std::vector<std::string> typeStrings;
    for (u8 i = 0; i < 18; i++)
    {
        typeStrings.push_back(static_cast<pksm::Type>(i).localize(pksm::Language::ENG));
    }
    

    brls::List *personalList = new brls::List();
    personalList->addView(new brls::ListItem(std::to_string(u16(this->pkm->species()))));
    if (this->pkm->generation() == pksm::Generation::NINE) {
        pksm::PKX& pkmBase = *this->pkm;
        // pksm::PK9& pkm9 = static_cast<pksm::PK9&>(*this->pkm);
        brls::SelectListItem *teraTypeOrig = new brls::SelectListItem("Original TeraType",typeStrings,u8(static_cast<pksm::PK9&>(pkmBase).teraTypeOriginal()));
        brls::SelectListItem *teraTypeOver = new brls::SelectListItem("Override TeraType",typeStrings,u8(static_cast<pksm::PK9&>(pkmBase).teraTypeOverride()));
        
        personalList->addView(teraTypeOrig);
        personalList->addView(teraTypeOver);
        
    }
    this->addTab("Personal",personalList);

    this->addSeparator();

    brls::List *evList = new brls::List();
    brls::List *ivList = new brls::List();

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

        brls::IntegerInputListItem *evInputItem = new brls::IntegerInputListItem(statName,this->pkm->ev(stat),"Enter a value between 0 a 252","",3);
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

        brls::IntegerInputListItem *ivInputItem = new brls::IntegerInputListItem(statName,this->pkm->iv(stat),"Enter a value between 0 and 31","",3);
        ivInputItem->getClickEvent()->subscribe([this,ivInputItem,stat,&save,&unsavedChanges](brls::View *v){
            u8 value = std::stoi(ivInputItem->getValue());

            if (value > 31) {
                ivInputItem->setValue(std::to_string(31));
            } else if (value < 0) {
                ivInputItem->setValue(std::to_string(0));
            } else {
                this->pkm->iv(stat,value);

                if (this->isParty) {
                    save->pkm(*this->pkm,this->slot);
                } else {
                    save->pkm(*this->pkm,this->box,this->slot,false);
                }
                
                unsavedChanges = true;
            }
        });


        evList->addView(evInputItem);
        ivList->addView(ivInputItem);
    }
    
    this->addTab("EVs",evList);
    this->addTab("IVs",ivList);
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