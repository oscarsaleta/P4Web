#ifndef SESSION_H
#define SESSION_H

#include "User.h"

#include <Wt/Auth/Login>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/backend/Sqlite3>

namespace dbo = Wt::Dbo;

typedef Wt::Auth::Dbo::UserDatabase<AuthInfo> UserDatabase;

class Session
{
public:
    static void configureAuth();

    Session();
    ~Session();

    Wt::Auth::AbstractUserDatabase& users();
    Wt::Auth::Login& login() { return login_; }

    dbo::ptr<User> user();
    dbo::ptr<User> user(const Wt::Auth::User& authUser);
    std::string userName() const;

    static const Wt::Auth::AuthService& auth();
    static const Wt::Auth::AbstractPasswordService& passwordAuth();
    static const std::vector<const Wt::Auth::OAuthService *>& oAuth();

private:
    dbo::backend::Sqlite3 sqlite3_;
    mutable dbo::Session session_;
    UserDatabase *users_;
    Wt::Auth::Login login_;
};

#endif /* SESSION_H */