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
#include "MyLogger.h"

#include <Wt/WApplication>
#include <Wt/WBootstrapTheme>
#include <Wt/WGroupBox>
#include <Wt/WLink>
#include <Wt/WMenu>
#include <Wt/WMenuItem>
#include <Wt/WMessageBox>
#include <Wt/WNavigationBar>
#include <Wt/WPopupMenu>
#include <Wt/WStackedWidget>
#include <Wt/WString>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

using namespace Wt;

MainUI::MainUI(WContainerWidget *root) : root_(root)
{
    root_->setId("root_");
    root_->setStyleClass(WString::fromUTF8("container"));
    root_->setInline(false);
}

void MainUI::setupUI(Auth::AuthWidget *authWidget)
{
    // set Bootstrap 3 theme
    WBootstrapTheme *theme = new WBootstrapTheme();
    theme->setVersion(WBootstrapTheme::Version3);
    WApplication::instance()->setTheme(theme);
    WApplication::instance()->setTitle(WString::tr("wapplication.settitle"));
    // add our own CSS file for some tweaks
    addAllStyleSheets();

    // title
    title_ = new WText(WString::tr("mainui.pagetitle"));
    title_->setId("title_");
    title_->setStyleClass("page-header center");
    root_->addWidget(title_);
    globalLogger__.debug("MainUI :: title set up");

    // left widget (file upload, input, buttons)
    globalLogger__.debug("MainUI :: creating HomeLeft...");
    leftContainer_ = new HomeLeft(root_,authWidget);
    globalLogger__.debug("MainUI :: HomeLeft created");

    // middle space (aligns other widgets to left and right)
    WContainerWidget *middleSpace_ = new WContainerWidget(root_);
    middleSpace_->setStyleClass("middle-box");

    // right widget (output text area, plots, legend)
    globalLogger__.debug("MainUI :: creating HomeRight...");
    rightContainer_ = new HomeRight(root_);
    globalLogger__.debug("MainUI :: HomeRight created");

    // connect signals sent from left to actions performed by right
    leftContainer_->evaluatedSignal().connect(rightContainer_,&HomeRight::readResults);
    leftContainer_->errorSignal().connect(rightContainer_,&HomeRight::printError);
    leftContainer_->onPlotSignal().connect(rightContainer_,&HomeRight::onPlot);
    globalLogger__.debug("MainUI :: signals connected");

    // add the widgets
    root_->addWidget(leftContainer_);
    root_->addWidget(middleSpace_);
    root_->addWidget(rightContainer_);

    globalLogger__.debug("MainUI :: MainUI set up");
}

void MainUI::addAllStyleSheets()
{
    WApplication::instance()->useStyleSheet(WLink("resources/main.css"));
}