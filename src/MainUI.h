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

#ifndef MAINUI_H_
#define MAINUI_H_

/*!
 * @brief Main container for UI
 * @file MainUI.h
 * @author Oscar Saleta
 */

#include <Wt/WContainerWidget>

#include "Session.h"

class HomeLeft;
class HomeRight;
class MyAuthWidget;

/**
 * Auxiliary class for organizing the different elements of the UI
 *
 * @class MainUI
 *
 * MainUI is a class without a constructor. We only need to call its setupUI()
 * function from #MyApplication and we get the different UI elements connected
 * through this class.
 *
 * This allows us to make some objects (#HomeRight) react to signals emitted by
 * others (#HomeLeft) without making everything global.
 */
class MainUI : public Wt::WContainerWidget
{
  public:
    /**
     * Constructor
     * @param parent Root container widget
     */
    MainUI(Wt::WContainerWidget *parent = 0);
    /**
     * Destructor
     */
    ~MainUI();

    /**
     * Sets up the UI of the web app from the root, to control every part of it
     *
     * This function is called from the constructor.
     */
    void setupUI();

    /**
     * Event triggered when a user authenticates or logs out
     *
     * Here we can customize the user experience for those who logged in, so
     * we can show them more options or tweak some program variables.
     */
    void onAuthEvent();

    /**
     * Function that handles an internal path change
     *
     * We use this to go in and out of the login page
     */
    void handlePathChange();

    void getMapleParams();

    std::vector<std::string> paramLabels_;
    std::vector<std::string> paramValues_;

  private:
    Session session_;
    MyAuthWidget *authWidget_;

    Wt::WContainerWidget *loginMessageContainer_;
    Wt::WText *loginText_;
    Wt::WAnchor *loginAnchor_;
    Wt::WAnchor *logoutAnchor_;

    Wt::WStackedWidget *mainStack_;
    Wt::WContainerWidget *pageContainer_;

    HomeLeft *leftContainer_;
    HomeRight *rightContainer_;

    Wt::WText *title_;
    Wt::WText *subtitle_;

    void setLoginIndicator(std::string userName);
    void setLogoutIndicator();
};

#endif // MAINUI_H