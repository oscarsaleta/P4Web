#ifndef USER_H
#define USER_H

#include <Wt/Dbo/Types>
#include <Wt/WGlobal>

namespace dbo = Wt::Dbo;

class User;
typedef Wt::Auth::Dbo::AuthInfo<User> AuthInfo;

class User {
public:
    std::string favouritePet;
    dbo::weak_ptr<AuthInfo> authInfo;
    template<class Action>
    void persist(Action &a)
    {
        dbo::field(a, favouritePet, "favourite_pet");
        dbo::hasOne(a, authInfo, "user");
    }
};

DBO_EXTERN_TEMPLATES(User);

#endif /* USER_H */