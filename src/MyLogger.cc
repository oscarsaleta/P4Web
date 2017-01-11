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
    configure("*");
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