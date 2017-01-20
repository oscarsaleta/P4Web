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