/*  P4 (Polynomial Planar Phase Portraits) WEB VERSION SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4Web
 *
 *  Copyright (C) 2016  O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
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