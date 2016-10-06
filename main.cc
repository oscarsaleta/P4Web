#include "MyApplication.h"

#include <Wt/Server>

Wt::WApplication *createApplication(const Wt::WEnvironment &env)
{
    return new MyApplication();
}

int main (int argc, char **argv)
{
    try {
        Wt::WServer server = new WServer(argc, argv, WTHTTP_CONFIGURATION);
        server.addEntryPoint(Wt::Application,createApplication);
        server.run();
    } catch (Wt::WServer::Exception &e) {
        std::cerr << e.what() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
    return 0;
}