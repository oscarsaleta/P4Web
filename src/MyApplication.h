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

#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

/*!
 * @brief Application class for the website backend
 * @file MyApplication.h
 * @author Oscar Saleta
 */

#include <Wt/WApplication>

#include "MainUI.h"
#include "Session.h"


/** 
 * Application class that links server and UI
 * 
 * @class MyApplication
 *
 * This class simply creates the application framework for
 * P4Web to run, and then creates an object of type #MainUI
 * to hold the UI elements of the webpage.
 */
class MyApplication : public Wt::WApplication
{
public:
    /** 
     * Constructor for MyApplication
     *
     * Simply runs #MainUI::setupUI() to generate a UI object
     */
    MyApplication(const Wt::WEnvironment &env);

    /** 
     * Destructor for MyApplication
     */
    ~MyApplication();
    
    void authEvent();

private:
    MainUI *mainUI_;
    Session session_;
};

#endif // MYAPPLICATION_H