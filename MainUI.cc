#include "MainUI.h"

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

#include "HomeLeft.h"
#include "HomeRight.h"

using namespace Wt;

void MainUI::setupUI(WContainerWidget *pageRoot)
{
    // set Bootstrap 3 theme
    WBootstrapTheme *theme = new WBootstrapTheme();
    theme->setVersion(WBootstrapTheme::Version3);
    WApplication::instance()->setTheme(theme);
    WApplication::instance()->setTitle("P4 Web v0.1 alpha");

    addAllStyleSheets();

    // get root page
    root_ = (pageRoot);
    root_->setId("root_");
    root_->setStyleClass(WString::fromUTF8("container"));
    root_->setInline(false);


    /* start of navbar*/

    // define navbar
    navbar_ = new WNavigationBar(root_);
    navbar_->setId("navbar_");
    navbar_->setStyleClass(WString::fromUTF8("navbar navbar-inverse"));
    navbar_->setInline(false);
    navbar_->setTitle("P4","#");
    navbar_->setResponsive(true);

    // navbar left menu
    nvLeftMenu_ = new WMenu(new WStackedWidget(),root_);
    nvLeftMenu_->contentsStack()->setId("nvLeftMenu_stackedWidget");
    nvLeftMenu_->contentsStack()->setStyleClass(WString::fromUTF8("container-fluid"));
    root_->addWidget(nvLeftMenu_->contentsStack());
    navbar_->addMenu(nvLeftMenu_,AlignLeft);
    nvLeftMenu_->setId("nvLeftMenu_");
    nvLeftMenu_->setInline(true);

    // left menu home button
    nvLeftMenuHomeContainer_ = new WContainerWidget(root_);
    nvLeftMenuHomeItem_ = nvLeftMenu_->addItem(WString::fromUTF8("Home"),nvLeftMenuHomeContainer_);
    nvLeftMenuHomeItem_->setId("nvLeftMenuHomeItem_");
    nvLeftMenuHomeItem_->setStyleClass(WString::fromUTF8(""));
    nvLeftMenuHomeItem_->setInline(false);
    nvLeftMenu_->setItemDisabled(0, false);
    nvLeftMenu_->setItemHidden(0, false);

    // left menu plot button
    nvLeftMenuPlotContainer_ = new WContainerWidget(root_);
    nvLeftMenuPlotItem_ = nvLeftMenu_->addItem(WString::fromUTF8("Plot"),nvLeftMenuPlotContainer_);
    nvLeftMenuPlotItem_->setId("nvLeftMenuPlotItem_");
    nvLeftMenuPlotItem_->setStyleClass(WString::fromUTF8(""));
    nvLeftMenuPlotItem_->setInline(false);
    nvLeftMenu_->setItemDisabled(1, false);
    nvLeftMenu_->setItemHidden(1, false);

    // left menu settings button
    nvLeftMenuSettingsContainer_ = new WContainerWidget(root_);
    nvLeftMenuSettingsItem_ = nvLeftMenu_->addItem(WString::fromUTF8("Settings"),nvLeftMenuSettingsContainer_);
    nvLeftMenuSettingsItem_->setId("nvLeftMenuSettingsItem_");
    nvLeftMenuSettingsItem_->setStyleClass(WString::fromUTF8(""));
    nvLeftMenuSettingsItem_->setInline(false);
    nvLeftMenu_->setItemDisabled(2, false);
    nvLeftMenu_->setItemHidden(2, false);

    // navbar right menu
    nvRightMenu_ = new WMenu();
    navbar_->addMenu(nvRightMenu_,AlignRight);

    nvRightPopup_ = new WPopupMenu();
    nvRightPopupMenuHelpDocItem_ = nvRightPopup_->addItem(WString::fromUTF8("Documentation"));
    nvRightPopupMenuHelpDocItem_->setLink(WLink("#"));
    nvRightPopup_->addSeparator();
    nvRightPopupMenuHelpAboutItem_ = nvRightPopup_->addItem(WString::fromUTF8("About P4..."));
    nvRightPopupMenuHelpAboutItem_->setLink(WLink("#"));

    nvRightPopup_->itemSelected().connect(std::bind([=] (Wt::WMenuItem *item) {
        Wt::WMessageBox *messageBox = new Wt::WMessageBox
        ("Help",
         Wt::WString::fromUTF8("<p>Showing Help: {1}</p>").arg(item->text()),
         Wt::Information, Wt::Ok);

        messageBox->buttonClicked().connect(std::bind([=] () {
        delete messageBox;
        }));

        messageBox->show();
    }, std::placeholders::_1));

    nvRightPopupButton_ = new WMenuItem("Help");
    nvRightPopupButton_->setMenu(nvRightPopup_);
    nvRightMenu_->addItem(nvRightPopupButton_);

    /* end of navbar */


    leftContainer_ = new HomeLeft(root_);

    WContainerWidget *middleSpace_ = new WContainerWidget(root_);
    middleSpace_->setStyleClass("middle-box");

    rightContainer_ = new HomeRight(root_);

    leftContainer_->evaluated().connect(rightContainer_,&HomeRight::readResults);

    root_->addWidget(leftContainer_);
    root_->addWidget(middleSpace_);
    root_->addWidget(rightContainer_);
}

void MainUI::addAllStyleSheets()
{
    WApplication::instance()->useStyleSheet(WLink("resources/main.css"));
}