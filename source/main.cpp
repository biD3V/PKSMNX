/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
    Copyright (C) 2019  p-sam

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

#include <switch.h>

#include <borealis.hpp>
#include <string>

#include "mainView.hpp"

namespace i18n = brls::i18n; // for loadTranslations() and getStr()
using namespace i18n::literals; // for _i18n

int main(int argc, char* argv[])
{
    // Init the app
    brls::Logger::setLogLevel(brls::LogLevel::ERROR);

    accountInitialize(AccountServiceType_Administrator);

    i18n::loadTranslations();
    if (!brls::Application::init("PKSMNX"))
    {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }

    brls::Application::pushView(new MainView());

    // Run the app
    while (brls::Application::mainLoop())
        ;

    
    // Exit
    return EXIT_SUCCESS;
}
