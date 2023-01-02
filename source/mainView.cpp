#include "mainView.hpp"

MainView::MainView() : AppletFrame(true,true)
{
    this->setTitle("PKSMNX");
    this->setIcon(BOREALIS_ASSET("icon/PKSMNX.jpg"));
    
    ProfilesTab *tab = new ProfilesTab();

    this->setContentView(tab);
}