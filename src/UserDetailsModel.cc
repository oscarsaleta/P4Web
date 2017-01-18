#include "UserDetailsModel.h"
#include "User.h"
#include "Session.h"


UserDetailsModel::UserDetailsModel(Session& session, Wt::WObject *parent)
    :   Wt::WFormModel(parent), session_(session)
{
}

void UserDetailsModel::save(const Wt::Auth::User& authUser)
{
    Wt::Dbo::ptr<User> user = session_.user(authUser);
}