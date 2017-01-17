#include "UserDetailsModel.h"
#include "User.h"
#include "Session.h"

using namespace Wt;

const WFormModel::Field UserDetailsModel::FavouritePetField = "favourite-pet";
UserDetailsModel::UserDetailsModel(Session& session, Wt::WObject *parent)
    :   WFormModel(parent), session_(session)
{
    addField(FavouritePetField, "PEEET");
}

void UserDetailsModel::save(const Auth::User& authUser)
{
    Dbo::prt<User> user = session_.user(authUser);
    user.modify()->favouritePet = valueText(FavouritePetField).toUTF8();
}