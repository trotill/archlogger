
/* c headers */
#include <syslog.h>
#include <stdarg.h>

/* c++ headers */
#include <stdexcept>

/* class header */
#include "log_syslog.hpp"

/* config header */
#include "../../config.h"

LogSyslog::LogSyslog(const bool verbose)
{
    setVerbose(verbose);
    openlog ( PACKAGE , LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
}

LogSyslog::~LogSyslog()
{
    closelog ();
}

void LogSyslog::setVerbose(const bool verbose)
{
    __verbose = verbose;
    setlogmask ( getVerbose() ? LOG_UPTO (LOG_DEBUG) : LOG_UPTO (LOG_NOTICE) );
}

bool LogSyslog::getVerbose() const
{
    return __verbose;
}

std::string LogSyslog::getPriorityAsString(const int priority)
{
    switch (priority)
    {
        case LOG_EMERG:
            // The message says the system is unusable.
            return "EMERGENCY: ";

        case LOG_ALERT:
            // Action on the message must be taken immediately.
            return "ALERT: ";

        case LOG_CRIT:
            // The message states a critical condition.
            return "CRITICAL: ";

        case LOG_ERR:
            // The message describes an error.
            return "ERROR: ";

        case LOG_WARNING:
            // The message is a warning.
            return "WARNING: ";

        case LOG_NOTICE:
            // The message describes a normal but important event. 
            return "NOTICE: ";

        case LOG_INFO:
            // The message is purely informational.
            return "INFO: ";

        case LOG_DEBUG:
            // The message is only for debugging purposes.
            return "DEBUG: ";

        default:
            // impossible, but if something corrupt memory.
            throw std::runtime_error( "Wrong log priority." );
    }
}

void LogSyslog::writeEmergency(const std::string& fmt, ...)
{
    if (fmt.empty()) return;

    va_list args;
    va_start(args, fmt);
    vsyslog (LOG_EMERG, (getPriorityAsString(LOG_EMERG) + fmt).c_str(), args);
    va_end(args);
}

void LogSyslog::writeAlert(const std::string& fmt, ...)
{
    if (fmt.empty()) return;

    va_list args;
    va_start(args, fmt);
    vsyslog (LOG_ALERT, (getPriorityAsString(LOG_ALERT) + fmt).c_str(), args);
    va_end(args);
}

void LogSyslog::writeCritical(const std::string& fmt, ...)
{
    if (fmt.empty()) return;

    va_list args;
    va_start(args, fmt);
    vsyslog (LOG_CRIT, (getPriorityAsString(LOG_CRIT) + fmt).c_str(), args);
    va_end(args);
}

void LogSyslog::writeError(const std::string& fmt, ...)
{
    if (fmt.empty()) return;

    va_list args;
    va_start(args, fmt);
    vsyslog (LOG_ERR, (getPriorityAsString(LOG_ERR) + fmt).c_str(), args);
    va_end(args);
}

void LogSyslog::writeWarning(const std::string& fmt, ...)
{
    if (fmt.empty()) return;

    va_list args;
    va_start(args, fmt);
    vsyslog (LOG_WARNING, (getPriorityAsString(LOG_WARNING) + fmt).c_str(), args);
    va_end(args);
}

void LogSyslog::writeNotice(const std::string& fmt, ...)
{
    if (fmt.empty()) return;

    va_list args;
    va_start(args, fmt);
    vsyslog (LOG_NOTICE, (getPriorityAsString(LOG_NOTICE) + fmt).c_str(), args);
    va_end(args);
}

void LogSyslog::writeInfo(const std::string& fmt, ...)
{
    if (fmt.empty()) return;

    va_list args;
    va_start(args, fmt);
    vsyslog (LOG_INFO, (getPriorityAsString(LOG_INFO) + fmt).c_str(), args);
    va_end(args);
}

void LogSyslog::writeDebug(const std::string& fmt, ...)
{
    if (fmt.empty()) return;

    va_list args;
    va_start(args, fmt);
    vsyslog (LOG_DEBUG, (getPriorityAsString(LOG_DEBUG) + fmt).c_str(), args);
    va_end(args);
}
