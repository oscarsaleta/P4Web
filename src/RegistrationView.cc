#include "RegistrationView.h"
#include "UserDetailsModel.h"

#include <Wt/WLineEdit>

using namespace Wt;

RegistrationView::RegistrationView(Session& session, Auth::AuthWidget *authWidget)
    :   Auth::RegistrationWidget(authWidget), session_(session)
{
    setTemplateText(tr("template.registration"));
    detailsModel_ = new UserDetailsModel(session_,this);
    updateView(detailsModel_);
}

WFormWidget *RegistrationView::createFormWidget(WFormModel::Field field)
{
    return Auth::RegistrationWidget::createFormWidget(field);
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