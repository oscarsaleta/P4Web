/*  This file is part of WP4 (http://github.com/oscarsaleta/WP4)
 *
 *  Copyright (C) 2016  O. Saleta
 *
 *  WP4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MyApplication.h"

#include "MainUI.h"
#include "MyLogger.h"

#include <Wt/WBootstrapTheme>

using namespace Wt;

MyApplication::MyApplication(const WEnvironment &env) :
    WApplication(env)
{
    // add resource files for strings and templates
    messageResourceBundle().use(appRoot()+"resources/xml/strings");
    messageResourceBundle().use(appRoot()+"resources/xml/templates");
    // install sendmail in order for this to work
    messageResourceBundle().use(appRoot()+"resources/xml/mail_strings");

    // set Bootstrap 3 theme
    WBootstrapTheme *theme = new WBootstrapTheme(this);
    theme->setVersion(WBootstrapTheme::Version3);
    setTheme(theme);
    setTitle(WString::tr("wapplication.settitle"));
    // add our own CSS file for some tweaks
    addAllStyleSheets();

    



    mainUI_ = new MainUI(root());

    globalLogger__.debug("MyApplication :: created correctly");
}


MyApplication::~MyApplication()
{
    delete mainUI_;
    globalLogger__.debug("MyApplication :: deleted correctly");
}


void MyApplication::addAllStyleSheets()
{
    useStyleSheet("resources/css/main.css");
}