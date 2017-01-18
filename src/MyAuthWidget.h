#ifndef AUTHWIDGET_H
#define AUTHWIDGET_H

#include <Wt/Auth/AuthWidget>

class Session;

class MyAuthWidget : public Wt::Auth::AuthWidget
{
public:
    MyAuthWidget (Session& session);

    virtual Wt::WWidget *createRegistrationView(const Wt::Auth::Identity& id);

private:
    Session& session_;
};

#endif // AUTHWIDGET_H