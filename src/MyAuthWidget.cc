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
#include "MyAuthWidget.h"

#include "RegistrationView.h"

using namespace Wt;

MyAuthWidget::MyAuthWidget(Session &session, WContainerWidget *parent)
    : Auth::AuthWidget(Session::auth(), session.users(), session.login(),
                       parent),
      session_(session)
{
    setTemplateText(tr("template.login"));
    updateView(new WFormModel(this));
}

WWidget *MyAuthWidget::createRegistrationView(const Auth::Identity &id)
{
/*    RegistrationView *w = new RegistrationView(session_, this);
    Auth::RegistrationModel *model = createRegistrationModel();
    model->setMinLoginNameLength(3);

    if (id.isValid())
        model->registerIdentified(id);

    w->setModel(model);
    return w;*/
}