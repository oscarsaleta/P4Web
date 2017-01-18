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

#ifndef MAINUI_H_
#define MAINUI_H_

/*!
 * @brief Main container for UI
 * @file MainUI.h
 * @author Oscar Saleta
 */

#include "MyAuthWidget.h"

#include <Wt/WContainerWidget>

class HomeLeft;
class HomeRight;

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
class MainUI
{
public:
    /**
     * Constructor
     * @param root Root container widget
     */
    MainUI(Wt::WContainerWidget *root = 0);
    ~MainUI();

    /** 
     * Sets up the UI of the web app from the root, to control every part of it
     *
     * This function is called from the #MyApplication when the program starts.
     * 
     * @param *pageRoot lowest level of the web page UI, will contain every other widget
     */
    void setupUI(MyAuthWidget *authWidget);

private:
    Wt::WContainerWidget *root_;
    
    //Wt::WNavigationBar *navbar_;

    /*Wt::WMenu *nvLeftMenu_;
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
    Wt::WMenuItem *nvRightPopupMenuHelpAboutItem_;*/

    HomeLeft *leftContainer_;
    HomeRight *rightContainer_;

    Wt::WText *title_;
    Wt::WText *subtitle_;
};

#endif // MAINUI_H