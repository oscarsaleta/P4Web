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
 * @mainpage P4 web version, a Wt-based C++ web application for computing planar polynomial phase portraits.
 * @author Oscar Saleta Reig
 *
 * This is a WIP. Main planned features are:
 * * Basic UI for introducing a vector field and viewing computed results (DONE)
 * * Interface for uploading scripts saved from P4 (DONE)
 * * Functionality for saving the vector field in a P4-friendly script (DONE)
 * * Basic plotting functionality: circle at infinity, singularities and separatrices (DONE)
 * * Full doxygen documentation
 * * Authentication framework (WIP)
 * * Advanced and unlocked settings for logged-in users
 * * Further functionality from P4: Poincaré-Lyapunov sphere, orbit integration, etc.
 *
 * The mathematical code of this application is based on the Qt-based C++ code of P4.
 * That said, the code has been heavily modified because this application uses <b>NO Qt</b>
 * classes or functions. The UI and backend elements have been build from zero using the
 * <b>Wt library</b> (<a href="https://www.webtoolkit.eu/wt">https://www.webtoolkit.eu/wt</a>),
 * because it is highly convenient to create web applications based on existing C++ code.
 */


#include "MyApplication.h"
#include "Session.h"

#include <Wt/WServer>

using namespace Wt;

WApplication *createApplication(const WEnvironment &env)
{
    return new MyApplication(env);
}

int main (int argc, char **argv)
{
    try {
        WServer server(argc, argv, WTHTTP_CONFIGURATION);
        server.addEntryPoint(Wt::Application,createApplication);
        Session::configureAuth();
        server.run();

    } catch (Wt::WServer::Exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (Wt::Dbo::Exception &e) {
        std::cerr << "Dbo exception: " << e.what() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
    return 0;
}