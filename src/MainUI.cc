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

#include "MainUI.h"

#include "HomeLeft.h"
#include "HomeRight.h"
#include "MyAuthWidget.h"
#include "MyLogger.h"
#include "file_tab.h"

#include <Wt/WAnchor>
#include <Wt/WApplication>
#include <Wt/WLink>
#include <Wt/WStackedWidget>
#include <Wt/WString>
#include <Wt/WText>

#include <Wt/Dbo/Transaction>

using namespace Wt;

MainUI::MainUI(WContainerWidget *parent) : WContainerWidget(parent)
{
    session_.login().changed().connect(this, &MainUI::onAuthEvent);

    setupUI();

    WApplication::instance()->internalPathChanged().connect(
        this, &MainUI::handlePathChange);
}

MainUI::~MainUI()
{
    if (loginText_ != nullptr) {
        delete loginText_;
        loginText_ = nullptr;
    }
    if (logoutAnchor_ != nullptr) {
        delete logoutAnchor_;
        logoutAnchor_ = nullptr;
    }
    if (loginAnchor_ != nullptr) {
        delete loginAnchor_;
        loginAnchor_ = nullptr;
    }
    if (loginMessageContainer_ != nullptr) {
        delete loginMessageContainer_;
        loginMessageContainer_ = nullptr;
    }
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (authWidget_ != nullptr) {
        delete authWidget_;
        authWidget_ = nullptr;
    }
    if (leftContainer_ != nullptr) {
        delete leftContainer_;
        leftContainer_ = nullptr;
    }
    if (rightContainer_ != nullptr) {
        delete rightContainer_;
        rightContainer_ = nullptr;
    }
    if (pageContainer_ != nullptr) {
        delete pageContainer_;
        pageContainer_ = nullptr;
    }
    if (mainStack_ != nullptr) {
        delete mainStack_;
        mainStack_ = nullptr;
    }
}

void MainUI::setupUI()
{
    setId("MainUI");
    setStyleClass("container");
    setInline(false);

    loginMessageContainer_ = new WContainerWidget(this);
    loginMessageContainer_->setId("loginMessageContainer_");
    addWidget(loginMessageContainer_);

    loginText_ = new WText(WString::tr("mainui.logintext-default"),
                           loginMessageContainer_);
    loginText_->setInline(true);
    loginMessageContainer_->addWidget(loginText_);

    logoutAnchor_ = new WAnchor("/login", "Logout", loginMessageContainer_);
    logoutAnchor_->setLink(WLink(WLink::InternalPath, "/login"));
    logoutAnchor_->setInline(true);
    logoutAnchor_->hide();
    loginMessageContainer_->addWidget(logoutAnchor_);

    loginAnchor_ = new WAnchor("/login", "Login", loginMessageContainer_);
    loginAnchor_->setLink(WLink(WLink::InternalPath, "/login"));
    loginAnchor_->setInline(true);
    loginMessageContainer_->addWidget(loginAnchor_);

    // title
    title_ = new WText(WString::tr("mainui.pagetitle"));
    title_->setId("title_");
    title_->setStyleClass("page-header center");
    addWidget(title_);
    g_globalLogger.debug("MainUI :: title set up");

    // this is used to change page content
    mainStack_ = new WStackedWidget();
    mainStack_->setId("mainStack_");
    addWidget(mainStack_);

    // login widget
    authWidget_ = new MyAuthWidget(session_, mainStack_);
    authWidget_->setId("authWidget_");
    authWidget_->model()->addPasswordAuth(&Session::passwordAuth());
    //authWidget_->model()->addOAuth(Session::oAuth());
    authWidget_->setRegistrationEnabled(true);

    // this holds the main page content
    pageContainer_ = new WContainerWidget(mainStack_);
    pageContainer_->setId("pageContainer_");

    WTemplate *t =
        new WTemplate(WString::tr("template.mainui"), pageContainer_);
    t->addFunction("id", WTemplate::Functions::id);

    // left widget (file upload, input, buttons)
    g_globalLogger.debug("MainUI :: creating HomeLeft...");
    leftContainer_ = new HomeLeft(pageContainer_);
    leftContainer_->parent_ = this;
    g_globalLogger.debug("MainUI :: HomeLeft created");
    t->bindWidget("left", leftContainer_);

    // right widget (output text area, plots, legend)
    g_globalLogger.debug("MainUI :: creating HomeRight...");
    rightContainer_ = new HomeRight(pageContainer_);
    g_globalLogger.debug("MainUI :: HomeRight created");
    t->bindWidget("right", rightContainer_);

    // copyright
    WText *copyright = new WText(WString::tr("mainui.disclaimer-copyright"));
    copyright->setId("copyright");
    addWidget(copyright);

    /* Connect signals sent from left to actions performed by right (and vice
     * versa)
     */
    // signals from HomeLeft
    leftContainer_->evaluatedSignal().connect(rightContainer_,
                                              &HomeRight::readResults);
    leftContainer_->errorSignal().connect(rightContainer_,
                                          &HomeRight::printError);
    leftContainer_->resetSignal().connect(rightContainer_, &HomeRight::onReset);
    leftContainer_->onPlotSphereSignal().connect(rightContainer_,
                                                 &HomeRight::onSpherePlot);
    leftContainer_->onPlotPlaneSignal().connect(rightContainer_,
                                                &HomeRight::onPlanePlot);
    leftContainer_->orbitIntegrateSignal().connect(
        rightContainer_, &HomeRight::onOrbitsIntegrate);
    leftContainer_->orbitDeleteSignal().connect(rightContainer_,
                                                &HomeRight::onOrbitsDelete);
    leftContainer_->gcfSignal().connect(rightContainer_, &HomeRight::onGcfEval);
    leftContainer_->addParameterSignal().connect(
        rightContainer_, &HomeRight::addParameterWithValue);

    // signals from HomeRight
    rightContainer_->sphereClickedSignal().connect(leftContainer_,
                                                   &HomeLeft::showOrbitsDialog);
    g_globalLogger.debug("MainUI :: signals connected");

    g_globalLogger.debug("MainUI :: MainUI set up");

    authWidget_->processEnvironment();
    handlePathChange();
}

void MainUI::onAuthEvent()
{
    if (session_.login().loggedIn()) {
        g_globalLogger.info("Auth :: User " + session_.userName() +
                            " logged in.");
        setLoginIndicator(session_.userName());
        leftContainer_->showSettings();
        rightContainer_->showParamsTab();
    } else {
        g_globalLogger.info("Auth :: User logged out.");
        leftContainer_->hideSettings();
        rightContainer_->hideParamsTab(true);
        setLogoutIndicator();
    }
    WApplication::instance()->setInternalPath("/", true);
}

void MainUI::handlePathChange()
{
    WApplication *app = WApplication::instance();

    if (app->internalPath() == "/login") {
        g_globalLogger.debug("MainUI :: handle internal path change /login");
        if (session_.login().loggedIn()) {
            session_.login().logout();
        } else
            mainStack_->setCurrentWidget(authWidget_);
    } else {
        mainStack_->setCurrentWidget(pageContainer_);
        g_globalLogger.debug("MainUI :: setting main page as current view");
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

void MainUI::getMapleParams()
{
    rightContainer_->refreshParamStringVectors();
    paramLabels_ = rightContainer_->paramLabels_;
    paramValues_ = rightContainer_->paramValues_;
}
