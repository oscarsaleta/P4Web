#include "MyApplication.h"

using namespace Wt;

MyApplication::MyApplication(const WEnvironment &env) : WApplication(env), mainUI_(new MainUI)
{
    messageResourceBundle().use(appRoot()+"resources/strings");
    mainUI_->setupUI(root());
}

MyApplication::~MyApplication()
{
    delete mainUI_;
}
