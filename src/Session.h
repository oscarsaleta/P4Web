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
#ifndef SESSION_H
#define SESSION_H

/*!
 * @brief File that contains the Session class
 * @file Session.h
 *
 * The Session class handles the user session for logged in users.
 */

#include "User.h"

#include <Wt/Auth/Login>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Sqlite3>
#include <Wt/Dbo/ptr>

namespace dbo = Wt::Dbo;

typedef Wt::Auth::Dbo::UserDatabase<AuthInfo>
    UserDatabase; ///< database of users

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
    Wt::Auth::AbstractUserDatabase &users();
    /**
     * Returns the login information
     */
    Wt::Auth::Login &login() { return login_; }

    /**
     * Access information about the current logged in user
     */
    dbo::ptr<User> user();
    /**
     * Access information about a specific user
     */
    dbo::ptr<User> user(const Wt::Auth::User &authUser);
    /**
     * Get the user name of the current logged in user
     * @return The user name as a string
     */
    std::string userName() const;

    /**
     * Returns the auth service object
     */
    static const Wt::Auth::AuthService &auth();
    /**
     * Returns the password service object
     */
    static const Wt::Auth::AbstractPasswordService &passwordAuth();
    /**
     * Returns the oAuth services vector
     *
     * In our case, this does nothing because we don't use oAuth for now
     */
    static const std::vector<const Wt::Auth::OAuthService *> &oAuth();

  private:
    dbo::backend::Sqlite3 sqlite3_;
    mutable dbo::Session session_;
    UserDatabase *users_;
    Wt::Auth::Login login_;
};

#endif /* SESSION_H */