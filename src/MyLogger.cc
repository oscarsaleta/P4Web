/*  This file is part of WP4 (http://github.com/oscarsaleta/WP4)
 *
 *  Copyright (C) 2016  O. Saleta
 *
 *  WP4 is free software: you can redistribute it and/or modify
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
#include "MyLogger.h"

#include <Wt/WApplication>

using namespace Wt;

MyLogger globalLogger__("log.txt");

MyLogger::MyLogger(std::string fname)
{
    addField("datetime",false);
    addField("session",false);
    addField("type",false);
    addField("message",true);
    setFile(fname);
    configure("*"); // configure for showing all logs
}

MyLogger::~MyLogger()
{

}

void MyLogger::log(std::string type, std::string message)
{
    WLogEntry entry = globalLogger__.entry(type);
    entry << WLogger::timestamp << WLogger::sep
        << '[' << WApplication::instance()->sessionId() << ']' << WLogger::sep
        << '[' << type << ']' << WLogger::sep
        << message;
}

void MyLogger::debug(std::string message)
{
    log("debug",message);
}

void MyLogger::info(std::string message)
{
    log("info",message);
}

void MyLogger::warning(std::string message)
{
    log("warning",message);
}

void MyLogger::secure(std::string message)
{
    log("secure",message);
}

void MyLogger::error(std::string message)
{
    log("error",message);
}


void MyLogger::fatal(std::string message)
{
    log("fatal",message);
}