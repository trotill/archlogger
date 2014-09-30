#ifndef LOGGER_SYSLOG_LOG_SYSLOG_HPP
#define LOGGER_SYSLOG_LOG_SYSLOG_HPP

/* c++ headers */
#include <string>

class LogSyslog
{
private:
    bool __verbose;

    std::string getPriorityAsString(const int);

protected:
    void writeEmergency(const std::string&, ...);
    void writeAlert(const std::string&, ...);
    void writeCritical(const std::string&, ...);
    void writeError(const std::string&, ...);
    void writeWarning(const std::string&, ...);
    void writeNotice(const std::string&, ...);
    void writeInfo(const std::string&, ...);
    void writeDebug(const std::string&, ...);

public:
    LogSyslog(const bool = false);
    virtual ~LogSyslog();

    void setVerbose(const bool);
    bool getVerbose() const;
};

#endif /* LOGGER_SYSLOG_LOG_SYSLOG_HPP */