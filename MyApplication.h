#ifndef __MYAPPLICATION_H_INCLUDED__
#define __MYAPPLICATION_H_INCLUDED__

#include <Wt/WApplication>

#include "MainUI.h"



class MyApplication : public Wt::WApplication
{
public:
    MyApplication(const Wt::WEnvironment &env);
    ~MyApplication();
    
private:
    MainUI *mainUI_;
};

#endif // __MYAPPLICATION_H_INCLUDED__