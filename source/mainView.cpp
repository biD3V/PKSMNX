#include "mainView.h"

MainView::MainView() : AppletFrame(true,true)
{
    this->setTitle("PKSMNX");
    
    ProfilesTab *tab = new ProfilesTab();

    this->setContentView(tab);
}