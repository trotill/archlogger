
/* c headers */
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>

/* c++ headers */
#include <algorithm>
#include <iomanip>
#include <sstream>

/* boost headers */
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "boost/date_time/local_time/local_time.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/regex.hpp>

/* class headers */
#include "log_helper.hpp"

static const uint64_t ONE_KB_BI = 1024;
static const uint64_t ONE_MB_BI = ONE_KB_BI * ONE_KB_BI;
static const uint64_t ONE_GB_BI = ONE_MB_BI * ONE_KB_BI;

static const uint64_t ONE_SEC  = 1000;
static const uint64_t ONE_MIN  = 60 * ONE_SEC;
static const uint64_t ONE_HOUR = 60 * ONE_MIN;
static const uint64_t ONE_DAY  = 24 * ONE_HOUR;
static const uint64_t ONE_WEEK =  7 * ONE_DAY;

LogHelper::LogHelper()
{
    // nothing to do
}

LogHelper::~LogHelper()
{
    // nothing to do
}

std::string LogHelper::size2human(const uint64_t size, const bool extend) const
{
    std::ostringstream os;

    // append partition size
    if (extend) {
        if (size / ONE_GB_BI > 0) {
            os << std::fixed << std::setprecision(2) << ((double)size / (double)ONE_GB_BI) << "G";
        } else if (size / ONE_MB_BI > 0) {
            os << std::fixed << std::setprecision(2) << ((double)size / (double)ONE_MB_BI) << "M";
        } else if (size / ONE_KB_BI > 0) {
            os << std::fixed << std::setprecision(2) << ((double)size / (double)ONE_KB_BI) << "K";
        } else {
            os << (size);
        }
    } else {
        if ((size / ONE_GB_BI > 0) && (size % ONE_GB_BI == 0)) {
            os << (size / ONE_GB_BI) << "G";
        } else if ((size / ONE_MB_BI > 0) && (size % ONE_MB_BI == 0)) {
            os << (size / ONE_MB_BI) << "M";
        } else if ((size / ONE_KB_BI > 0) && (size % ONE_KB_BI == 0)) {
            os << (size / ONE_KB_BI) << "K";
        } else {
            os << (size);
        }
    }

    return os.str();
}

std::string LogHelper::time2human(const uint64_t time, const bool extend) const
{
    std::ostringstream os;

    // append partition size
    if (extend) {
        if (time / ONE_WEEK > 0) {
            os << std::setprecision(2) << ((double)time / (double)ONE_WEEK) << "w";
        } else if (time / ONE_DAY > 0) {
            os << std::setprecision(2) << ((double)time / (double)ONE_DAY) << "d";
        } else if (time / ONE_HOUR > 0) {
            os << std::setprecision(2) << ((double)time / (double)ONE_HOUR) << "h";
        } else if (time / ONE_MIN > 0) {
            os << std::setprecision(2) << ((double)time / (double)ONE_MIN) << "m";
        } else if (time / ONE_SEC > 0) {
            os << std::setprecision(2) << ((double)time / (double)ONE_SEC) << "s";
        } else {
            os << (time) << "ms";
        }
    } else {
        if ((time / ONE_WEEK > 0) && (time % ONE_WEEK == 0)) {
            os << (time / ONE_WEEK) << "w";
        } else if ((time / ONE_DAY > 0) && (time % ONE_DAY == 0)) {
            os << (time / ONE_DAY) << "d";
        } else if ((time / ONE_HOUR > 0) && (time % ONE_HOUR == 0)) {
            os << (time / ONE_HOUR) << "h";
        } else if ((time / ONE_MIN > 0) && (time % ONE_MIN == 0)) {
            os << (time / ONE_MIN) << "m";
        } else if ((time / ONE_SEC > 0) && (time % ONE_SEC == 0)) {
            os << (time / ONE_SEC) << "s";
        } else {
            os << (time) << "ms";
        }
    }

    return os.str();
}

uint64_t LogHelper::human2size(const std::string& value) const
{
    boost::regex expression("^([0-9]+)(g|m|k|b|(?:))$",
                            boost::regex::perl | boost::regex::icase);
    boost::cmatch matches;

    if (boost::regex_match(value.c_str(), matches, expression))
    {
        // have numbers
        if ((matches[1].matched) && (matches[2].matched))
        {
            std::string match_value(matches[1].first, matches[1].second);
            std::string match_size(matches[2].first, matches[2].second);

            uint64_t ret_value = 
                static_cast<uint64_t>(std::atol( match_value.c_str() ));

            if (match_size.empty() || boost::iequals(match_size, "b"))
                return ret_value;
            else if (boost::iequals(match_size, "k"))
                return ret_value * ONE_KB_BI;
            else if (boost::iequals(match_size, "m"))
                return ret_value * ONE_MB_BI;
            else if (boost::iequals(match_size, "g"))
                return ret_value * ONE_GB_BI;
        }
    }
    return 0;
}

uint64_t LogHelper::human2time(const std::string& value) const
{
    boost::regex expression("^([0-9]+)(w|d|h|m|s|ms|(?:))$",
                            boost::regex::perl | boost::regex::icase);
    boost::cmatch matches;

    if (boost::regex_match(value.c_str(), matches, expression))
    {
        // have numbers
        if (matches[1].matched && matches[2].matched)
        {
            std::string match_value(matches[1].first, matches[1].second);
            std::string match_time(matches[2].first, matches[2].second);

            uint64_t ret_value = 
                static_cast<uint64_t>(std::atol( match_value.c_str() ));

            if (match_time.empty() || boost::iequals(match_time, "ms"))
                return ret_value;
            else if (boost::iequals(match_time, "s"))
                return ret_value * ONE_SEC;
            else if (boost::iequals(match_time, "m"))
                return ret_value * ONE_MIN;
            else if (boost::iequals(match_time, "h"))
                return ret_value * ONE_HOUR;
            else if (boost::iequals(match_time, "d"))
                return ret_value * ONE_DAY;
            else if (boost::iequals(match_time, "w"))
                return ret_value * ONE_WEEK;
        }
    }

    return 0;
}

bool LogHelper::fileExists(const std::string& filename) const
{
    return (access( filename.c_str(), F_OK ) == 0);
}

bool LogHelper::fileReadable(const std::string& filename) const
{
    return (access( filename.c_str(), R_OK ) == 0);
}

bool LogHelper::fileWriteable(const std::string& filename) const
{
    return (access( filename.c_str(), W_OK ) == 0);
}

bool LogHelper::fileExecutable(const std::string& filename) const
{
    return (access( filename.c_str(), X_OK ) == 0);
}

bool LogHelper::pathIsDir(const std::string& path) const
{
    try
    {
        return boost::filesystem::is_directory(path);
    } catch (const boost::filesystem::filesystem_error& ex) {
    } catch (...) {
    }
    return false;
}

bool compareStrings(const DirInfo& i, const DirInfo& j)
{
    return boost::algorithm::lexicographical_compare(i.first, j.first);
}

boost::optional<DirInfoVec>
LogHelper::getDirInfo(const std::string& dirpath, const bool recursive)
{
#define DIR_INFO_EMPTY boost::optional<DirInfoVec>()

    if (dirpath.empty())
        return DIR_INFO_EMPTY;

    DIR *dir;
    struct dirent *dentry;

    struct stat file_info;
    int status = 0;

    std::string path;
    uint64_t    filesize;
    DirInfoVec  dirInfo;
    boost::optional<DirInfoVec> subdirInfo;

    dir = opendir(dirpath.c_str());
    if (!dir)
        return DIR_INFO_EMPTY;

    for (;;)
    {
        dentry = readdir(dir);
        if (!dentry)
            break;

        path = dirpath + "/" + dentry->d_name;

        // skip parent and current dir
        if ((strcmp(dentry->d_name, ".") == 0)
            || (strcmp(dentry->d_name, "..") == 0))
            continue;

        if (recursive && (dentry->d_type & DT_DIR)) {
            subdirInfo = getDirInfo(path);
            if (subdirInfo)
                dirInfo.insert(dirInfo.end(), subdirInfo.get().begin(), subdirInfo.get().end());
        }

        status = stat(path.c_str(), &file_info);
        // if can't get stat of file set UINT64_MAX
        filesize = (status == 0) ? file_info.st_size : UINT64_MAX;

        dirInfo.push_back(std::make_pair(path, filesize));
    }

    if (closedir (dir) != 0) {
        // some error, but don't care
    }

    std::sort(dirInfo.begin(), dirInfo.end(), compareStrings);

    return boost::optional<DirInfoVec>(dirInfo);

#undef DIR_INFO_EMPTY
}

uint64_t LogHelper::getFreeSpace(const std::string& path)
{
    struct statvfs info;
    for (uint8_t i = 0; i < UCHAR_MAX; ++i)
    {
        if ( statvfs(path.c_str(), &info) != -1 )
            return ( info.f_bsize * info.f_bfree );
    }
    return 0;
}

uint64_t LogHelper::getFreeSpaceNonRoot(const std::string& path)
{
    struct statvfs info;
    for (uint8_t i = 0; i < UCHAR_MAX; ++i)
    {
        if ( statvfs(path.c_str(), &info) != -1 )
            return ( info.f_bsize * info.f_bavail );
    }
    return 0;
}

boost::posix_time::ptime
LogHelper::getLocalTime() const
{
    return boost::posix_time::second_clock::local_time();
}

boost::posix_time::ptime
LogHelper::getUTCTime() const
{
    return boost::posix_time::second_clock::universal_time();
}

std::string LogHelper::getTimeZone() const
{
    tzset();
    return std::string(tzname[0]);
}

int LogHelper::cmdExec(const std::string& command)
{
    return (command.empty()) ? -1 : system(command.c_str());
}