#include "MyApplication.h"

#include <Wt/WServer>

using namespace Wt;

WApplication *createApplication(const WEnvironment &env)
{
    return new MyApplication(env);
}

int main (int argc, char **argv)
{
    try {
        WServer *server = new WServer(argc, argv, WTHTTP_CONFIGURATION);
        server->addEntryPoint(Wt::Application,createApplication);
        server->run();
    } catch (Wt::WServer::Exception &e) {
        std::cerr << e.what() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
    return 0;
}