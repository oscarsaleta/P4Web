#ifndef USER_H
#define USER_H

/*!
 * @brief File that contains the User class
 * @author Oscar Saleta
 */

#include <Wt/Dbo/Types>
#include <Wt/WGlobal>

namespace dbo = Wt::Dbo;

class User;
typedef Wt::Auth::Dbo::AuthInfo<User> AuthInfo;

/**
 * User class
 * @class User
 *
 * This class defines the fields that a user has, and
 * then calls Wt::Dbo to create the database and tables
 * if they don't exist.
 */
class User {
public:
    dbo::weak_ptr<AuthInfo> authInfo;   ///< auth info of the user
    /**
     * Make an action on the database for this user
     */
    template<class Action>
    void persist(Action &a)
    {
        dbo::hasOne(a, authInfo, "user");
    }
};

DBO_EXTERN_TEMPLATES(User);

#endif /* USER_H */