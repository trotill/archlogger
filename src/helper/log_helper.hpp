#ifndef LOGGER_HELPER_LOG_HELPER_HPP
#define LOGGER_HELPER_LOG_HELPER_HPP

/* limits headers */
#include "log_limits.h"

/* c headers */
#include <time.h>

/* c++ headers */
#include <map>
#include <string>
#include <vector>

/* boost headers */
#include <boost/optional.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

typedef std::pair<std::string, uint64_t> DirInfo;
typedef std::vector<DirInfo> DirInfoVec;

class LogHelper
{
protected:

    // covert human-readable size to bytes and vise-versa
    std::string size2human(const uint64_t, const bool = false) const;
    uint64_t human2size(const std::string&) const;

    // covert human-readable time to milliseconds and vise-versa
    std::string time2human(const uint64_t, const bool = false) const;
    uint64_t human2time(const std::string&) const;

    // for file system
    bool fileExists(const std::string&) const;
    bool fileReadable(const std::string&) const;
    bool fileWriteable(const std::string&) const;
    bool fileExecutable(const std::string&) const;

    bool pathIsDir(const std::string&) const;
    boost::optional<DirInfoVec>
    getDirInfo(const std::string&, const bool = true);

    // get free space on partition include root reserve
    uint64_t getFreeSpace(const std::string&);
    // get free space on partition exclude root reserve
    uint64_t getFreeSpaceNonRoot(const std::string&);

    // for datetime
    boost::posix_time::ptime
    getLocalTime() const;
    boost::posix_time::ptime
    getUTCTime() const;
    std::string getTimeZone() const;

    // execute external command
    int cmdExec(const std::string&);

public:
    LogHelper();
    virtual ~LogHelper();
};

#endif /* LOGGER_HELPER_LOG_HELPER_HPP */