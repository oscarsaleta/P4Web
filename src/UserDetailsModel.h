#ifndef USERDETAILSMODEL_H
#define USERDETAILSMODEL_H

/*!
 * @brief This file contains the class UserDetailsModel
 * @author Oscar Saleta
 */

#include <Wt/WFormModel>

class Session;

/**
 * Model for storing custom user details
 * @class UserDetailsModel
 *
 * The default form model for registering a user
 * only saves username+password+email. Reimplementing
 * this class, we can register whatever information we
 * need.
 *
 * In our case, (for now) we use the default fields.
 */
class UserDetailsModel : public Wt::WFormModel
{
public:
    /**
     * Constructor method
     * @param session A #Session object, which contains the information
     * about the current session
     * @param parent The object that created this form model
     */
    UserDetailsModel(Session& session, Wt::WObject *parent = 0);

    /**
     * Save user to database
     * @param user User info to save in database
     */
    void save(const Wt::Auth::User& user);

private:
    Session &session_;
};

#endif // USERDETAILSMODEL_H