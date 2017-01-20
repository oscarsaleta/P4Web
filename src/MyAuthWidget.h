#ifndef AUTHWIDGET_H
#define AUTHWIDGET_H

#include <Wt/Auth/AuthWidget>
#include "Session.h"

#include <Wt/WContainerWidget>
#include <Wt/WWidget>

class MyAuthWidget : public Wt::Auth::AuthWidget
{
public:
    MyAuthWidget (Session& session, Wt::WContainerWidget *parent = 0);

    virtual Wt::WWidget *createRegistrationView(const Wt::Auth::Identity& id);

private:
    Session& session_;
};

#endif // AUTHWIDGET_H