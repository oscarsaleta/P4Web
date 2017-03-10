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
#ifndef USER_H
#define USER_H

/*!
 * @brief File that contains the User class
 * @file User.h
 *
 * This class defines what a user looks like (what fields
 * are registered in the database)
 */

#include <Wt/Dbo/Types>
#include <Wt/WGlobal>

namespace dbo = Wt::Dbo;

class User;
typedef Wt::Auth::Dbo::AuthInfo<User>
    AuthInfo; ///< authentication info for a user

/**
 * User class
 * @class User
 *
 * This class defines the fields that a user has, and
 * then calls Wt::Dbo to create the database and tables
 * if they don't exist.
 */
class User
{
  public:
    dbo::weak_ptr<AuthInfo> authInfo; ///< auth info of the user
                                      /**
                                       * Make an action on the database for this user
                                       */
    template <class Action> void persist(Action &a)
    {
        dbo::hasOne(a, authInfo, "user");
    }
};

/**
 * Define several templates for the User to
 * communicate with the database
 */
DBO_EXTERN_TEMPLATES(User);

#endif /* USER_H */