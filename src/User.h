#ifndef USER_H
#define USER_H

#include <Wt/Dbo/Types>
#include <Wt/WGlobal>

namespace dbo = Wt::Dbo;

class User;
typedef Wt::Auth::Dbo::AuthInfo<User> AuthInfo;

class User {
public:
    template<class Action>
    void persist(Action &a){}
};

DBO_EXTERN_TEMPLATES(User);

#endif /* USER_H */