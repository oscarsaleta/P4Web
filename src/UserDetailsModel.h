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
#ifndef USERDETAILSMODEL_H
#define USERDETAILSMODEL_H

/*!
 * @brief This file contains the class UserDetailsModel
 * @file UserDetailsModel.h
 *
 * This class implements a WFormModel used to customise
 * the information required from a user at registration.
 * This class is not actually needed for the moment, but
 * this may change in the future.
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
    UserDetailsModel(Session &session, Wt::WObject *parent = 0);

    /**
     * Save user to database
     * @param user User info to save in database
     */
    void save(const Wt::Auth::User &user);

  private:
    Session &session_;
};

#endif // USERDETAILSMODEL_H