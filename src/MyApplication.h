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

#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

/*!
 * @brief Application class for the website backend
 * @file MyApplication.h
 * @author Oscar Saleta
 */

#include <Wt/WApplication>

class MainUI;

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

  private:
    MainUI *mainUI_;

    void addAllStyleSheets();
};

#endif // MYAPPLICATION_H