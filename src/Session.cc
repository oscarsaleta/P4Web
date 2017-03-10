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
#include "Session.h"

#include "MyLogger.h"

#include <Wt/WApplication>

#include <Wt/Auth/AuthService>
#include <Wt/Auth/Dbo/AuthInfo>
#include <Wt/Auth/Dbo/UserDatabase>
#include <Wt/Auth/FacebookService>
#include <Wt/Auth/GoogleService>
#include <Wt/Auth/HashFunction>
#include <Wt/Auth/PasswordService>
#include <Wt/Auth/PasswordStrengthValidator>
#include <Wt/Auth/PasswordVerifier>

using namespace Wt;

namespace
{
class MyOAuth : public std::vector<const Auth::OAuthService *>
{
  public:
    ~MyOAuth()
    {
        for (unsigned i = 0; i < size(); ++i)
            delete (*this)[i];
    }
};

Auth::AuthService myAuthService;
Auth::PasswordService myPasswordService(myAuthService);
MyOAuth myOAuthServices;
}

void Session::configureAuth()
{
    myAuthService.setAuthTokensEnabled(true, "p4webcookie");
    myAuthService.setEmailVerificationEnabled(true);
    myAuthService.setEmailVerificationRequired(false);

    Auth::PasswordVerifier *verifier = new Auth::PasswordVerifier();
    verifier->addHashFunction(new Auth::BCryptHashFunction(7));

    myPasswordService.setVerifier(verifier);
    myPasswordService.setStrengthValidator(
        new Auth::PasswordStrengthValidator());
    myPasswordService.setAttemptThrottlingEnabled(true);

    /*if (Auth::GoogleService::configured())
        myOAuthServices.push_back(new Auth::GoogleService(myAuthService));
    if (Auth::FacebookService::configured())
        myOAuthServices.push_back(new Auth::FacebookService(myAuthService));*/

    for (unsigned i = 0; i < myOAuthServices.size(); ++i)
        myOAuthServices[i]->generateRedirectEndpoint();
}

Session::Session() : sqlite3_(WApplication::instance()->appRoot() + "auth.db")
{
    session_.setConnection(sqlite3_);
    sqlite3_.setProperty("show-queries", "false");

    session_.mapClass<User>("user");
    session_.mapClass<AuthInfo>("auth_info");
    session_.mapClass<AuthInfo::AuthIdentityType>("auth_identity");
    session_.mapClass<AuthInfo::AuthTokenType>("auth_token");

    users_ = new UserDatabase(session_);
    Dbo::Transaction transaction(session_);
    try {
        session_.createTables();
        globalLogger__.info("Created database.");
    } catch (...) {
        // std::cerr << e.what() << std::endl;
        globalLogger__.info("Using existing database");
    }
    transaction.commit();
}

Session::~Session() { delete users_; }

dbo::ptr<User> Session::user()
{
    if (login_.loggedIn()) {
        return user(login_.user());
    } else
        return dbo::ptr<User>();
}

dbo::ptr<User> Session::user(const Wt::Auth::User &authUser)
{
    dbo::ptr<AuthInfo> authInfo = users_->find(authUser);
    dbo::ptr<User> user = authInfo->user();
    if (!user) {
        user = session_.add(new User());
        authInfo.modify()->setUser(user);
    }
    return user;
}

std::string Session::userName() const
{
    if (login_.loggedIn())
        return login_.user().identity(Auth::Identity::LoginName).toUTF8();
    else
        return std::string();
}

Auth::AbstractUserDatabase &Session::users() { return *users_; }

const Auth::AuthService &Session::auth() { return myAuthService; }

const Auth::AbstractPasswordService &Session::passwordAuth()
{
    return myPasswordService;
}

const std::vector<const Auth::OAuthService *> &Session::oAuth()
{
    return myOAuthServices;
}