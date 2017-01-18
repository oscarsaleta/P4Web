/*  P4 (Polynomial Planar Phase Portraits) WEB VERSION SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4Web
 *
 *  Copyright (C) 2016  O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
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
#include "MyAuthWidget.h"
#include "MyLogger.h"

#include <Wt/Auth/AuthModel>
#include <Wt/Auth/AuthWidget>
#include <Wt/Auth/PasswordService>

#include <Wt/WBootstrapTheme>

using namespace Wt;

MyApplication::MyApplication(const WEnvironment &env)
    :   WApplication(env), session_(appRoot() + "auth.db")
{
    session_.login().changed().connect(this, &MyApplication::authEvent);

    // set Bootstrap 3 theme
    WBootstrapTheme *theme = new WBootstrapTheme(this);
    theme->setVersion(WBootstrapTheme::Version3);
    setTheme(theme);
    setTitle(WString::tr("wapplication.settitle"));
    // add our own CSS file for some tweaks
    addAllStyleSheets();
    messageResourceBundle().use(appRoot()+"resources/strings");
    messageResourceBundle().use(appRoot()+"resources/templates");


    // login widget
    MyAuthWidget *authWidget = new MyAuthWidget(session_);
    authWidget->setId("authWidget");
    authWidget->model()->addPasswordAuth(&Session::passwordAuth());
    authWidget->model()->addOAuth(Session::oAuth());
    authWidget->setRegistrationEnabled(true);

    authWidget->processEnvironment();
    //root()->addWidget(authWidget);


    mainUI_ = new MainUI(root());
    mainUI_->setupUI(authWidget);

    globalLogger__.debug("MyApplication :: created correctly");
}


MyApplication::~MyApplication()
{
    delete mainUI_;
    globalLogger__.debug("MyApplication :: deleted correctly");
}

void MyApplication::authEvent()
{
    if (session_.login().loggedIn()) {
        globalLogger__.info("User "+session_.login().user().id()+" logged in.");
        Wt::Dbo::Transaction t(session_);
        dbo::ptr<User> user = session_.user();
    } else
        globalLogger__.info("User logged out.");
}

void MyApplication::addAllStyleSheets()
{
    useStyleSheet("resources/main.css");
}