#ifndef MYLOGGER_H
#define MYLOGGER_H

/*!
 * @brief Declares a WLogger class (which logs events from the web)
 * @file MyLogger.h
 * @author Oscar Saleta Reig
 */

#include <Wt/WLogger>

/**
 * Logger class with routines to print log messages to a file
 * @class MyLogger
 *
 * There are different levels of logging: debug, info, warning, secure,
 * error and fatal.
 *
 * We use a global MyLogger object to use the same logger in every
 * unit of the application.
 */
class MyLogger : public Wt::WLogger
{
public:
    /**
     * Creator method
     * @param fname Name of log file
     */
    MyLogger(std::string fname);
    /**
     * Destructor method
     */
    ~MyLogger();

    /**
     * Log a debug message
     * @param message Text to log
     *
     * These are debugging messages, which
     * are not visible by default.
     */
    void debug(std::string message);
    /**
     * Log an info message
     * @param message Text to log
     *
     * These are normal messages to inform of actions
     * performed by the application or user.
     */
    void info(std::string message);
    /**
     * Log a warning message
     * @param message Text to log
     *
     * These are warnings that inform of a potential
     * missusage of the application.
     */
    void warning(std::string message);
    /**
     * Log a secure message
     * @param message Text to log
     *
     * These are messages related to security issues.
     */
    void secure(std::string message);
    /**
     * Log an error message
     * @param message Text to log
     *
     * These are messages caused by a wrong usage of the API.
     */
    void error(std::string message);
    /**
     * Log a fatal message
     * @param message Text to log
     *
     * These messages are treated like fatal errors, execution
     * is halted.
     */
    void fatal(std::string message);

private:
    void log(std::string type, std::string message);
};

extern MyLogger globalLogger__; ///< Global logger object

#endif // MYLOGGER_H 
