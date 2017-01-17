#include "AuthWidget.h"

#include "RegistrationView.h"

using namespace Wt;

AuthWidget::AuthWidget (Session& session)
    : Auth::AuthWidget(Session::auth(), session.users(), session.login()),
    session_(session)
{

}

WWidget *AuthWidget::CreateRegistrationView (const Auth::Identity& id)
{
    RegistrationView *w = new RegistrationView(session_, this);
    Auth::RegistrationModel *model = createRegistrationModel();
    model->setMinLoginNameLength(3);

    if (id.isValid())
        model->registerIdentified(id);

    w->setModel(model);
    return w;
}