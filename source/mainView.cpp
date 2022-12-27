#include "mainView.h"

MainView::MainView() : TabFrame()
{
    ProfilesTab *tab = new ProfilesTab();

    this->addTab("Profiles", tab);
}