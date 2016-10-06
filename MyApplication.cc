#include "MyApplication.h"

using namespace Wt;

MyApplication::MyApplication(const WEnvironment &env) : WApplication(env), mainUI_(new MainUI)
{
    mainUI_->setupUI(root());
}

MyApplication::~MyApplication()
{
    delete mainUI_;
}
