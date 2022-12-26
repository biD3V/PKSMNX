#include "profilesTab.h"

ProfilesTab::ProfilesTab()
{
    this->testLabel = new brls::Label(brls::LabelStyle::REGULAR, "Test Label", true);
    this->addView(testLabel);
}

ProfilesTab::~ProfilesTab()
{
    
}