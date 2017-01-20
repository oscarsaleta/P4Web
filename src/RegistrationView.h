#ifndef REGISTRATIONVIEW_H
#define REGISTRATIONVIEW_H

/*!
 * @brief This file contains the RegistrationView class
 * @author Oscar Saleta
 */

#include <Wt/Auth/RegistrationWidget>

class Session;
class UserDetailsModel;

/**
 * Custom Wt::RegistrationWidget for a custom Wt::AuthWidget
 * @class RegistrationView
 *
 * This class is needed to implement our own registration fields,
 * through a UserDetailsModel object, although for now the 
 * implementation works exactly as a regular RegistrationWidget.
 */
class RegistrationView : public Wt::Auth::RegistrationWidget
{
public:
    /**
     * Constructor method
     * @param session A #Session object, which contains the information
     * about the current session
     * @param authWidget The authentication widget that created this 
     * registration view
     */
    RegistrationView(Session& session, Wt::Auth::AuthWidget *authWidget = 0);

    /**
     * Create the registration form
     * @param  field Field to add to the form
     * @return       Wt::WFormWidget which contains the registration form
     */
    virtual Wt::WFormWidget *createFormWidget(Wt::WFormModel::Field field);

protected:
    /**
     * Field validator
     * @return @c true if all fields are correct, @c false otherwise
     */
    virtual bool validate();
    /**
     * Register the user details to the database
     * @param user User details
     */
    virtual void registerUserDetails(Wt::Auth::User& user);

private:
    Session& session_;

    UserDetailsModel *detailsModel_;
};

#endif // REGISTRATIONVIEW_H