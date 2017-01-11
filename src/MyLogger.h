#ifndef MYLOGGER_H
#define MYLOGGER_H

#include <Wt/WLogger>

class MyLogger : public Wt::WLogger
{
public:
    MyLogger(std::string fname);
    ~MyLogger();

    void debug(std::string);
    void info(std::string);
    void warning(std::string);
    void secure(std::string);
    void error(std::string);
    void fatal(std::string);

private:
    void log(std::string type, std::string message);
};

extern MyLogger globalLogger__;
#endif