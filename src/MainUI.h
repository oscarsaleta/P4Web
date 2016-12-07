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

#endif // MAINUI_H