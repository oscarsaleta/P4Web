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

#include "MainUI.h"

#include "file_tab.h"
#include "HomeLeft.h"
#include "HomeRight.h"
#include "MyAuthWidget.h"
#include "MyLogger.h"

#include <Wt/WAnchor>
#include <Wt/WApplication>
#include <Wt/WGroupBox>
#include <Wt/WLink>
#include <Wt/WMenu>
#include <Wt/WMenuItem>
#include <Wt/WMessageBox>
#include <Wt/WPopupMenu>
#include <Wt/WStackedWidget>
#include <Wt/WString>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

#include <Wt/Dbo/Transaction>

using namespace Wt;

MainUI::MainUI(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    session_.login().changed().connect(this,&MainUI::onAuthEvent);

    setupUI();

    WApplication::instance()->internalPathChanged().connect(this, &MainUI::handlePathChange);
    
}

MainUI::~MainUI()
{
    delete leftContainer_;
    delete rightContainer_;
    delete authWidget_;
}

void MainUI::setupUI()
{
    setId("MainUI");
    setStyleClass("container");
    setInline(false);

    loginMessageContainer_ = new WContainerWidget(this);
    loginMessageContainer_->setId("loginMessageContainer_");
    addWidget(loginMessageContainer_);

    loginText_ = new WText(WString::tr("mainui.logintext-default"),loginMessageContainer_);
    loginText_->setInline(true);
    loginMessageContainer_->addWidget(loginText_);

    logoutAnchor_ = new WAnchor("/login","Logout",loginMessageContainer_);
    logoutAnchor_->setLink(WLink(WLink::InternalPath,"/login"));
    logoutAnchor_->setInline(true);
    logoutAnchor_->hide();
    loginMessageContainer_->addWidget(logoutAnchor_);

    loginAnchor_ = new WAnchor("/login","Login",loginMessageContainer_);
    loginAnchor_->setLink(WLink(WLink::InternalPath,"/login"));
    loginAnchor_->setInline(true);
    loginMessageContainer_->addWidget(loginAnchor_);


    // title
    title_ = new WText(WString::tr("mainui.pagetitle"));
    title_->setId("title_");
    title_->setStyleClass("page-header center");
    addWidget(title_);
    globalLogger__.debug("MainUI :: title set up");

    // this is used to change page content
    mainStack_ = new WStackedWidget();
    mainStack_->setId("mainStack_");
    addWidget(mainStack_);

    // login widget
    authWidget_ = new MyAuthWidget(session_, mainStack_);
    authWidget_->setId("authWidget_");
    authWidget_->model()->addPasswordAuth(&Session::passwordAuth());
    authWidget_->model()->addOAuth(Session::oAuth());
    authWidget_->setRegistrationEnabled(true);

    // this holds the main page content
    pageContainer_ = new WContainerWidget(mainStack_);
    pageContainer_->setId("pageContainer_");

    WTemplate *t = new WTemplate(WString::tr("template.mainui"),pageContainer_);
    t->addFunction("id",WTemplate::Functions::id);

    // left widget (file upload, input, buttons)
    globalLogger__.debug("MainUI :: creating HomeLeft...");
    leftContainer_ = new HomeLeft(pageContainer_);
    globalLogger__.debug("MainUI :: HomeLeft created");
    t->bindWidget("left",leftContainer_);

    // right widget (output text area, plots, legend)
    globalLogger__.debug("MainUI :: creating HomeRight...");
    rightContainer_ = new HomeRight(pageContainer_);
    globalLogger__.debug("MainUI :: HomeRight created");
    t->bindWidget("right",rightContainer_);

    // connect signals sent from left to actions performed by right (and vice versa)
    leftContainer_->evaluatedSignal().connect(rightContainer_,&HomeRight::readResults);
    leftContainer_->errorSignal().connect(rightContainer_,&HomeRight::printError);
    leftContainer_->resetSignal().connect(rightContainer_,&HomeRight::onReset);
    leftContainer_->onPlotSphereSignal().connect(rightContainer_,&HomeRight::onSpherePlot);
    leftContainer_->onPlotPlaneSignal().connect(rightContainer_,&HomeRight::onPlanePlot);
    leftContainer_->orbitIntegrateSignal().connect(rightContainer_,&HomeRight::onOrbitsIntegrate);
    leftContainer_->orbitDeleteSignal().connect(rightContainer_,&HomeRight::onOrbitsDelete);
    rightContainer_->sphereClickedSignal().connect(leftContainer_,&HomeLeft::showOrbitsDialog);
    globalLogger__.debug("MainUI :: signals connected");

    globalLogger__.debug("MainUI :: MainUI set up");

    authWidget_->processEnvironment();
    handlePathChange();
}

void MainUI::onAuthEvent()
{
    if (session_.login().loggedIn()) {
        globalLogger__.info("Auth :: User "+session_.userName()+" logged in.");
        setLoginIndicator(session_.userName());
        leftContainer_->showSettings();
    } else {
        globalLogger__.info("Auth :: User logged out.");
        leftContainer_->hideSettings();
        setLogoutIndicator();
    }
    WApplication::instance()->setInternalPath("/",true);
}

void MainUI::handlePathChange()
{
    WApplication *app = WApplication::instance();

    if (app->internalPath() == "/login") {
        globalLogger__.debug("MainUI :: handle internal path change /login");
        if (session_.login().loggedIn()) {
            session_.login().logout();
        } else
            mainStack_->setCurrentWidget(authWidget_);
    } else {
        mainStack_->setCurrentWidget(pageContainer_);
        globalLogger__.debug("MainUI :: setting main page as current view");
    }
}

void MainUI::setLoginIndicator(std::string userName)
{
    loginText_->setText(WString::tr("mainui.logintext-user").arg(userName));
    loginAnchor_->hide();
    loginText_->show();
    logoutAnchor_->show();
}

void MainUI::setLogoutIndicator()
{
    loginText_->setText(WString::tr("mainui.logintext-default"));
    logoutAnchor_->hide();
    loginAnchor_->show();
}