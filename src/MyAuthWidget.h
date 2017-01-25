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
#ifndef AUTHWIDGET_H
#define AUTHWIDGET_H

/*!
 * @brief This file contains the MyAuthWidget class definition
 * @author Oscar Saleta
 */

#include <Wt/Auth/AuthWidget>
#include "Session.h"

#include <Wt/WContainerWidget>
#include <Wt/WWidget>

/**
 * Widget that manages authentication into the website
 * @class MyAuthWidget
 *
 * This authentication widget manages login/logout and also
 * register/forgotten password. The usernames/passwords are
 * stored and hashed in a local sqlite3 database.
 */
class MyAuthWidget : public Wt::Auth::AuthWidget
{
public:
    /**
     * Constructor method
     * @param session A #Session object, which contains the information
     * about the current session
     * @param parent The widget in which this widget is embedded
     */
    MyAuthWidget (Session& session, Wt::WContainerWidget *parent = 0);

    /**
     * Create a custom registration view
     * @param  id An identifier for the current session
     * @return    A widget containing the registration view (usually a
     * floating dialog)
     *
     * This function is needed to modify the registration model, in order
     * to be able to register usernames with less than 4 characters.
     */
    virtual Wt::WWidget *createRegistrationView(const Wt::Auth::Identity& id);

private:
    Session& session_;
};

#endif // AUTHWIDGET_H