/*  P4 (Polynomial Planar Phase Portraits) WEB VERSION SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4Web
 *
 *  Copyright (C) 2016  O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * @mainpage P4 web version, a C++/Wt web application
 * @author Oscar Saleta Reig
 */

/*!
 * @file main.c 
 * @brief Main P4 web program.
 * @author Oscar Saleta Reig
 */

#include "MyApplication.h"

#include <Wt/WServer>

using namespace Wt;

/**
 * @param &env WEnvironment variable (passed automatically)
 * @return MyApplication object (the web app itself)
 * @brief This function creates the main application (the web app)
 *
 * This function is called automatically by the server when the entry
 * point is added and we call run() on it.
 */
WApplication *createApplication(const WEnvironment &env)
{
    return new MyApplication(env);
}

/**
 * @param @c argc count of how many arguments there are in @p **argv
 * @param @c **argv arguments to the program
 * @return @c 0 always
 * @brief Main function. Creates the server and makes it run
 *
 * This function will create a loop and will keep running until
 * the server is turned off or some error occurs.
 */
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