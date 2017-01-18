#ifndef REGISTRATIONVIEW_H
#define REGISTRATIONVIEW_H

#include <Wt/Auth/RegistrationWidget>

class Session;
class UserDetailsModel;

class RegistrationView : public Wt::Auth::RegistrationWidget
{
public:
    RegistrationView(Session& session, Wt::Auth::AuthWidget *authWidget = 0);

    virtual Wt::WFormWidget *createFormWidget(Wt::WFormModel::Field field);

protected:
    virtual bool validate();
    virtual void registerUserDetails(Wt::Auth::User& user);

private:
    Session& session_;

    UserDetailsModel *detailsModel_;
};

#endif // REGISTRATIONVIEW_H