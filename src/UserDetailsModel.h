#ifndef USERDETAILSMODEL_H
#define USERDETAILSMODEL_H

#include <Wt/WFormModel>

class Session;

class UserDetailsModel : public Wt::WFormModel
{
public:
    UserDetailsModel(Session& session, Wt::WObject *parent = 0);

    void save(const Wt::Auth::User& user);

private:
    Session &session_;
};

#endif // USERDETAILSMODEL_H