#ifndef __MAINUI_H_INCLUDED__
#define __MAINUI_H_INCLUDED__

#include <Wt/WContainerWidget>

class HomeLeft;
class HomeRight;

class MainUI
{
public:
    void setupUI(Wt::WContainerWidget *pageRoot);

private:
    Wt::WContainerWidget *root_;
    
    Wt::WNavigationBar *navbar_;

    Wt::WMenu *nvLeftMenu_;
    Wt::WContainerWidget *nvLeftMenuHomeContainer_;
    Wt::WMenuItem *nvLeftMenuHomeItem_;
    Wt::WContainerWidget *nvLeftMenuPlotContainer_;
    Wt::WMenuItem *nvLeftMenuPlotItem_;
    Wt::WContainerWidget *nvLeftMenuSettingsContainer_;
    Wt::WMenuItem *nvLeftMenuSettingsItem_;

    Wt::WMenu *nvRightMenu_;
    Wt::WPopupMenu *nvRightPopup_;
    Wt::WMenuItem *nvRightPopupButton_;
    Wt::WMenuItem *nvRightPopupMenuHelpDocItem_;
    Wt::WMenuItem *nvRightPopupMenuHelpAboutItem_;

    HomeLeft *leftContainer_;
    HomeRight *rightContainer_;



    void addAllStyleSheets();
};

#endif // __MAINUI_H_INCLUDED__