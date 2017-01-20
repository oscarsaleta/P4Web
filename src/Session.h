#ifndef SESSION_H
#define SESSION_H

/*!
 * @brief File that contains the Session class
 * @author Oscar Saleta
 */

#include "User.h"

#include <Wt/Auth/Login>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/backend/Sqlite3>

namespace dbo = Wt::Dbo;

typedef Wt::Auth::Dbo::UserDatabase<AuthInfo> UserDatabase;

/**
 * Class that holds all the authentication information of every session
 * @class Session
 *
 * This class manages the authentication information of the session
 * (login, connection to database, etc). This is the base class
 * needed by the authentication/registration widgets.
 */
class Session
{
public:
    /**
     * Configures the authentication service options
     */
    static void configureAuth();

    /**
     * Constructor method
     */
    Session();
    /**
     * Destructor method
     */
    ~Session();

    /**
     * Returns a reference to the user database
     */
    Wt::Auth::AbstractUserDatabase& users();
    /**
     * Returns the login information
     */
    Wt::Auth::Login& login() { return login_; }

    /**
     * Access information about the current logged in user
     */
    dbo::ptr<User> user();
    /**
     * Access information about a specific user
     */
    dbo::ptr<User> user(const Wt::Auth::User& authUser);
    /**
     * Get the user name of the current logged in user
     * @return The user name as a string
     */
    std::string userName() const;

    /**
     * Returns the auth service object
     */
    static const Wt::Auth::AuthService& auth();
    /**
     * Returns the password service object
     */
    static const Wt::Auth::AbstractPasswordService& passwordAuth();
    /**
     * Returns the oAuth services vector
     *
     * In our case, this does nothing because we don't use oAuth for now
     */
    static const std::vector<const Wt::Auth::OAuthService *>& oAuth();

private:
    dbo::backend::Sqlite3 sqlite3_;
    mutable dbo::Session session_;
    UserDatabase *users_;
    Wt::Auth::Login login_;
};

#endif /* SESSION_H */