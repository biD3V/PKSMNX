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
    for (u8 i = 0; i < 18; i++) {
        typeStrings.push_back(static_cast<pksm::Type>(i).localize(pksm::Language::ENG));
    }

    std::vector<std::string> natureStrings;
    for (u8 i = 0; i < 25; i++) {
        natureStrings.push_back(static_cast<pksm::Nature>(i).localize(pksm::Language::ENG));
    }
    
    brls::List *personalList = new brls::List();
    personalList->addView(new brls::ListItem(this->pkm->species().localize(pksm::Language::ENG)));

    brls::SelectListItem *nature = new brls::SelectListItem("Nature",natureStrings,u8(this->pkm->nature()));
    nature->getValueSelectedEvent()->subscribe([this,nature,&save,&unsavedChanges](int i){
        pksm::Nature selectedNature = static_cast<pksm::Nature>(i);
        this->pkm->nature(selectedNature);
        addChangesToSave(save);
        unsavedChanges = true;
    });
    personalList->addView(nature);

    std::vector<std::string> abilityStrings;
    for (u8 i = 0; i < 3; i++) {
        if (this->pkm->abilities(i) != pksm::Ability::None)
            abilityStrings.push_back(this->pkm->abilities(i).localize(pksm::Language::ENG));
    }

    brls::SelectListItem *ability = new brls::SelectListItem("Ability",abilityStrings,getAbilityListNumber(this->pkm->abilityNumber() >> 1));
    ability->getValueSelectedEvent()->subscribe([this,ability,&save,&unsavedChanges](int i){
        int selectedAbility = i;
        if (this->pkm->abilities(1) == pksm::Ability::None && i == 1) selectedAbility = 2;

        this->pkm->setAbility(selectedAbility);
        addChangesToSave(save);
        unsavedChanges = true;
    });
    personalList->addView(ability);

    if (this->pkm->generation() == pksm::Generation::NINE) {
        pksm::PKX& pkmBase = *this->pkm;
        // pksm::PK9& pkm9 = static_cast<pksm::PK9&>(*this->pkm);
        brls::SelectListItem *teraType = new brls::SelectListItem("Tera Type",typeStrings,u8(static_cast<pksm::PK9&>(pkmBase).getTeraType()));
        teraType->getValueSelectedEvent()->subscribe([this,teraType,&save,&unsavedChanges](int i){
            pksm::Type selectedType = static_cast<pksm::Type>(teraType->getSelectedValue());
            static_cast<pksm::PK9&>(*this->pkm).setTeraType(selectedType);
            addChangesToSave(save);
            unsavedChanges = true;
        });
        personalList->addView(teraType);
        
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
                addChangesToSave(save);
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
                addChangesToSave(save);
                unsavedChanges = true;
            }
        });


        evList->addView(evInputItem);
        ivList->addView(ivInputItem);
    }
    
    this->addTab("EVs",evList);
    this->addTab("IVs",ivList);
}

u8 PKMInfoView::getAbilityListNumber(u8 n) {
    if (this->pkm->abilities(1) == pksm::Ability::None) {
        if (n >= 1) return n-1;
    } else {
        return n;
    }
}



void PKMInfoView::addChangesToSave(std::shared_ptr<pksm::Sav> &save) {
    if (this->isParty)
        save->pkm(*this->pkm,this->slot);
    else
        save->pkm(*this->pkm,this->box,this->slot,false);
}