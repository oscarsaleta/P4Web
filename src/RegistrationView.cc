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
#include "RegistrationView.h"
#include "UserDetailsModel.h"

#include <Wt/WLineEdit>

using namespace Wt;

RegistrationView::RegistrationView(Session& session, Auth::AuthWidget *authWidget)
    :   Auth::RegistrationWidget(authWidget), session_(session)
{
    //setTemplateText(tr("Wt.Auth.template.registration"));
    detailsModel_ = new UserDetailsModel(session_,this);
    updateView(detailsModel_);
}

WFormWidget *RegistrationView::createFormWidget(WFormModel::Field field)
{
    return (WFormWidget*) Auth::RegistrationWidget::createFormWidget(field);
}

bool RegistrationView::validate()
{
    bool result = Auth::RegistrationWidget::validate();

    updateModel(detailsModel_);
    if (!detailsModel_->validate())
        result = false;
    updateView(detailsModel_);
    return result;
}

void RegistrationView::registerUserDetails(Auth::User& user)
{
    detailsModel_->save(user);
}