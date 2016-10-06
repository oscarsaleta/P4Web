#ifndef __MYAPPLICATION_H_INCLUDED__
#define __MYAPPLICATION_H_INCLUDED__

#include <Wt/WApplication>

class MainUI;

class MyApplication : Wt::WApplication
{
public:
    MyApplication(const Wt::WEnvironment &e);
    ~MyApplication();
    
private:
    MainUI mainUI_;
};

#endif // __MYAPPLICATION_H_INCLUDED__