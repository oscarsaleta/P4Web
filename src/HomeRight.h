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

#ifndef HOMERIGHT_H
#define HOMERIGHT_H

#include "win_sphere.h"

#include <Wt/WContainerWidget>

#include <Wt/WPushButton>
#include <Wt/WString>
#include <Wt/WTabWidget>
#include <Wt/WTextArea>
#include <Wt/WToolBar>

class HomeRight : public Wt::WContainerWidget
{
public:
    HomeRight(Wt::WContainerWidget *parent = 0);
    ~HomeRight();

    void readResults(int,std::string);
    void printError(std::string);
    void onPlot(std::string);

private:
    Wt::WTabWidget *tabWidget_;

    Wt::WContainerWidget *outputContainer_;
    Wt::WTextArea *outputTextArea_;
    Wt::WString outputTextAreaContent_;
    Wt::WToolBar *outputButtonsToolbar_;
    Wt::WPushButton *fullResButton_;
    Wt::WPushButton *finResButton_;
    Wt::WPushButton *infResButton_;
    Wt::WPushButton *clearButton_;


    std::string fileName_;
    std::string fullResults_;
    std::string finResults_;
    std::string infResults_;

    WWinSphere *sphere_;

    Wt::WContainerWidget *plotContainer_;

    int flag_;

    void fullResults();
    void showFinResults();
    void showInfResults();
    
};

#endif // HOMERIGHT_H