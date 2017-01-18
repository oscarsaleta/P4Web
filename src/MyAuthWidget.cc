#include "MyAuthWidget.h"

#include "RegistrationView.h"
#include "Session.h"

using namespace Wt;

MyAuthWidget::MyAuthWidget (Session& session)
    : Auth::AuthWidget(Session::auth(), session.users(), session.login()),
    session_(session)
{
    setTemplateText(tr("template.login"));
    updateView(new WFormModel(this));
}

WWidget *MyAuthWidget::createRegistrationView (const Auth::Identity& id)
{
    RegistrationView *w = new RegistrationView(session_, this);
    Auth::RegistrationModel *model = createRegistrationModel();
    model->setMinLoginNameLength(3);

    if (id.isValid())
        model->registerIdentified(id);

    w->setModel(model);
    return w;
}