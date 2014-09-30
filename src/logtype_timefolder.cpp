
/* c headers */
#include <sys/stat.h>

/* c++ headers */
#include <sstream>
#include <iomanip>

/* boost headers */
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/regex.hpp>

/* class headers */
#include "logtype_timefolder.hpp"
#include "ini/settings.hpp"


static void *thread_gc (void *arg)
{
    LogTypeTimeFolder *logtype_tf = (LogTypeTimeFolder *)arg;
    if (logtype_tf == NULL) return NULL;

    uint64_t timeout = logtype_tf->getFreqFreeSize() * 1000;

    while ( (logtype_tf != NULL)
            && logtype_tf->isThreadRun()
            && !logtype_tf->isTerminate())
    {
        usleep(timeout);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        logtype_tf->clean();
        // call second time for remove empty dir
        logtype_tf->clean();
        logtype_tf->reindex();
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
    return NULL;
}

static void *thread_create_dir (void *arg)
{
    LogTypeTimeFolder *logtype_tf = (LogTypeTimeFolder *)arg;
    if (logtype_tf == NULL) return NULL;

    uint64_t timeout = logtype_tf->getCreateDirInterval() * 1000;

    while ( (logtype_tf != NULL)
            && logtype_tf->isThreadRun()
            && !logtype_tf->isTerminate())
    {
        usleep(timeout);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        logtype_tf->setCurrentLogDir();
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
    return NULL;
}

static void *thread_create_file (void *arg)
{
    LogTypeTimeFolder *logtype_tf = (LogTypeTimeFolder *)arg;

    uint64_t timeout = logtype_tf->getCreateFileInterval() * 1000;

    while ( (logtype_tf != NULL)
            && logtype_tf->isThreadRun()
            && !logtype_tf->isTerminate())
    {
        usleep(timeout);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        logtype_tf->setCurrentLogFile();
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
    return NULL;
}

static void *thread_udp_send (void *arg)
{
    LogTypeTimeFolder *logtype_tf = (LogTypeTimeFolder *)arg;
    if (logtype_tf == NULL) return NULL;

    uint8_t swahello[10];
    swahello[0] = 0x00;
    swahello[1] = 0x00;
    swahello[2] = 0x00;
    swahello[3] = 0x06;
    swahello[4] = 0x68;
    swahello[5] = 0x65;
    swahello[6] = 0x6C;
    swahello[7] = 0x6C;
    swahello[8] = 0x6F;
    swahello[9] = 0x00;

    boost::system::error_code ignored_error;

    // wait SWAHello
    while ((logtype_tf != NULL) 
            && !logtype_tf->isTerminate()
            && (logtype_tf->getUdpSocket() != NULL)
            && (logtype_tf->getUdpRemoteEndpoint() != NULL))
    {
        logtype_tf->getUdpSocket()->send_to(
                boost::asio::buffer(swahello),
                *(logtype_tf->getUdpRemoteEndpoint()),
                0,
                ignored_error);
        usleep(10 * 1000 * 1000);
    }

    return NULL;
}

static void *thread_udp_recv (void *arg)
{
    LogTypeTimeFolder *logtype_tf = (LogTypeTimeFolder *)arg;
    if (logtype_tf == NULL) return NULL;

    boost::system::error_code error;

    uint8_t buffer[65535];

    // wait SWAHello
    while ((logtype_tf != NULL)
            && !logtype_tf->isTerminate()
            && (logtype_tf->getUdpSocket() != NULL)
            && (logtype_tf->getUdpRemoteEndpoint() != NULL))
    {
        memset(buffer, 0, sizeof(buffer));
        logtype_tf->getUdpSocket()->receive_from(
                boost::asio::buffer(buffer),
                *(logtype_tf->getUdpRemoteEndpoint()),
                0,
                error);
        if (error && error != boost::asio::error::message_size) {
            // throw boost::system::system_error(error);
        } else {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            logtype_tf->parseSWA(buffer);
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        }
    }

    return NULL;
}


LogTypeTimeFolder::LogTypeTimeFolder(const LogType& parent):
    LogType(parent)
{
    pthread_mutex_init(&lock_file, NULL);
    pthread_mutex_init(&lock_dir, NULL);

	__path = ini::settings::tf::path;
	__createDirInterval = ini::settings::tf::createDirInterval;
	__createFileInterval = ini::settings::tf::createFileInterval;
	__maxSize = ini::settings::tf::maxSize;
	__minFree = ini::settings::tf::minFree;
	__freqFreeSize = ini::settings::tf::freqFreeSize;
	__delimeter = ini::settings::tf::delimeter;

    __datetimeSync = false;

    __csvWorker = boost::make_shared<CSVWorker>();
    if (!__csvWorker) {
        writeError ("CSV Worker not allocated");
        // impossible, but if something corrupt memory.
        throw std::runtime_error( "CSV Worker not allocated" );
    }

    __csvSetMaxIndex = true;
}

LogTypeTimeFolder::~LogTypeTimeFolder()
{
    __csvWorker.reset();
}

void LogTypeTimeFolder::setPath(const std::string& path)
{
	__path = path;
}

void LogTypeTimeFolder::setCreateDirInterval(const uint64_t createDirInterval)
{
	__createDirInterval = createDirInterval;
}

void LogTypeTimeFolder::setCreateFileInterval(const uint64_t createFileInterval)
{
	__createFileInterval = createFileInterval;
}

void LogTypeTimeFolder::setMaxSize(const uint64_t maxSize)
{
	__maxSize = maxSize;
}

void LogTypeTimeFolder::setMinFree(const uint64_t minFree)
{
	__minFree = minFree;
}

void LogTypeTimeFolder::setFreqFreeSize(const uint64_t freqFreeSize)
{
	__freqFreeSize = freqFreeSize;
}

void LogTypeTimeFolder::setDelimeter(const char delimeter)
{
	__delimeter = delimeter;
}

std::string LogTypeTimeFolder::getPath() const
{
	return __path;
}

uint64_t LogTypeTimeFolder::getCreateDirInterval() const
{
	return __createDirInterval;
}

std::string LogTypeTimeFolder::getCreateDirIntervalAsString() const
{
    return time2human(getCreateDirInterval());
}

uint64_t LogTypeTimeFolder::getCreateFileInterval() const
{
	return __createFileInterval;
}

std::string LogTypeTimeFolder::getCreateFileIntervalAsString() const
{
    return time2human(getCreateFileInterval());
}

uint64_t LogTypeTimeFolder::getMaxSize() const
{
	return __maxSize;
}

std::string LogTypeTimeFolder::getMaxSizeAsString() const
{
    return size2human(getMaxSize());
}

uint64_t LogTypeTimeFolder::getMinFree() const
{
	return __minFree;
}

std::string LogTypeTimeFolder::getMinFreeAsString() const
{
    return size2human(getMinFree());
}

uint64_t LogTypeTimeFolder::getFreqFreeSize() const
{
	return __freqFreeSize;
}

std::string LogTypeTimeFolder::getFreqFreeSizeAsString() const
{
    return size2human(getFreqFreeSize());
}

char LogTypeTimeFolder::getDelimeter() const
{
	return __delimeter;
}

bool LogTypeTimeFolder::parsePath()
{
    setPath( iniGetValue( ini::TF_SECTION,
        ini::VAR_TF_SWA_PATH, ini::settings::tf::path ) );

    if (iniGetError())
    {
        writeError( "TimeFolder: Can't get 'Log folder path'" );
        return false;
    }

    if ( !pathIsDir(getPath()) )
    {
        boost::filesystem::create_directories(getPath());
        if ( !pathIsDir(getPath()) ) {
            writeError( "TimeFolder: 'Log folder path' invalid" );
            return false;
        }
    }

    if ( !fileReadable( getPath() ))
    {
        writeError( "TimeFolder: 'Log folder path' not read permission" );
        return false;
    }

    if ( !fileWriteable( getPath() ))
    {
        writeError( "TimeFolder: 'Log folder path' not write permission" );
        return false;
    }

    if ( !fileExecutable( getPath() ))
    {
        writeError( "TimeFolder: 'Log folder path' not execute permission" );
        return false;
    }

    writeInfo( "TimeFolder: Log folder path = %s", getPath().c_str() );

    return true;
}

bool LogTypeTimeFolder::parseCreateDirInterval()
{
    setCreateDirInterval( human2time( iniGetValue(  ini::TF_SECTION,
        ini::VAR_TF_SWA_CREATE_DIR_INTERVAL, time2human(ini::settings::tf::createDirInterval) )));

    if (iniGetError())
    {
        writeError( "TimeFolder: Can't get 'Create directory interval'" );
        return false;
    }

    if ( getCreateDirInterval() == 0 )
    {
        writeError( "TimeFolder: 'Create directory interval' invalid" );
        return false;
    }

    writeInfo ( "TimeFolder: 'Create directory interval' = %s (%lu ms)",
                getCreateDirIntervalAsString().c_str(),
                getCreateDirInterval());

    return true;
}

bool LogTypeTimeFolder::parseCreateFileInterval()
{
    setCreateFileInterval( human2time( iniGetValue(  ini::TF_SECTION,
        ini::VAR_TF_SWA_CREATE_FILE_INTERVAL, time2human(ini::settings::tf::createFileInterval) )));

    if (iniGetError())
    {
        writeError( "TimeFolder: Can't get 'Create file interval'" );
        return false;
    }

    if ( getCreateFileInterval() == 0 )
    {
        writeError( "TimeFolder: 'Create file interval' invalid" );
        return false;
    }

    if ( getCreateFileInterval() >= getCreateDirInterval() )
    {
        writeError( "TimeFolder: 'Create file interval' must be less than 'Create directory interval'" );
        return false;
    }

    writeInfo ( "TimeFolder: Create file interval = %s (%lu ms)",
                getCreateFileIntervalAsString().c_str(),
                getCreateFileInterval());

    return true;
}

bool LogTypeTimeFolder::parseMaxSize()
{
    setMaxSize( human2size( iniGetValue(  ini::TF_SECTION,
        ini::VAR_TF_SWA_MAX_SIZE, size2human(ini::settings::tf::maxSize) )));

    if (iniGetError())
    {
        writeError( "TimeFolder: Can't get 'Max Size'" );
        return false;
    }

    if ( getMaxSize() == 0 )
    {
        writeError( "TimeFolder: 'Max Size' invalid" );
        return false;
    }

    writeInfo ( "TimeFolder: Max Size = %s (%lu bytes)",
                getMaxSizeAsString().c_str(),
                getMaxSize());

    return true;
}

bool LogTypeTimeFolder::parseMinFree()
{
    setMinFree( human2size( iniGetValue(  ini::TF_SECTION,
        ini::VAR_TF_SWA_MIN_FREE, size2human(ini::settings::tf::minFree) )));

    if (iniGetError())
    {
        writeError( "TimeFolder: Can't get 'Min Free'" );
        return false;
    }

    if ( getMaxSize() == 0 )
    {
        writeError( "TimeFolder: 'Min Free' invalid" );
        return false;
    }

    writeInfo ( "TimeFolder: Min Free = %s (%lu bytes)",
                getMinFreeAsString().c_str(),
                getMinFree());

    return true;
}

bool LogTypeTimeFolder::parseFreqFreeSize()
{
    setFreqFreeSize( human2time( iniGetValue(  ini::TF_SECTION,
        ini::VAR_TF_SWA_FREQ_FREE_SIZE, time2human(ini::settings::tf::freqFreeSize) )));

    if (iniGetError())
    {
        writeError( "TimeFolder: Can't get 'Freq Free Size'" );
        return false;
    }

    if ( getFreqFreeSize() == 0 )
    {
        writeError( "TimeFolder: 'Freq Free Size' invalid" );
        return false;
    }

    if ( getFreqFreeSize() >= getCreateFileInterval() )
    {
        writeError( "TimeFolder: 'Freq Free Size' must be less than 'Create file interval'" );
        return false;
    }

    writeInfo ( "TimeFolder: Freq Free Size = %s (%lu ms)",
                getFreqFreeSizeAsString().c_str(),
                getFreqFreeSize());

    return true;
}

bool LogTypeTimeFolder::parseDelimeter()
{
    setDelimeter( iniGetValue( ini::TF_SECTION,
        ini::VAR_TF_SWA_DELIMETER, ini::settings::tf::delimeter ) );

    if (iniGetError())
    {
        writeError( "TimeFolder: Can't get 'Delimeter'" );
        return false;
    }

    if ( getDelimeter() == 0x00)
    {
        writeError( "TimeFolder: 'Delimeter' invalid" );
        return false;
    }

    writeInfo( "TimeFolder: Delimeter = '%c'", getDelimeter() );

    return true;
}

void LogTypeTimeFolder::setDatetimeSync()
{
    __datetimeSync = (getLocalTime().date().year() >= 2014);
}

bool LogTypeTimeFolder::getDatetimeSync() const
{
    return __datetimeSync;
}

void LogTypeTimeFolder::setLastDirIndex(const uint16_t last_dir_index)
{
    __last_dir_index = last_dir_index;
}

uint16_t LogTypeTimeFolder::getLastDirIndex() const
{
    return __last_dir_index;
}

bool LogTypeTimeFolder::setCurrentLogDir()
{
    pthread_mutex_lock(&lock_file);
    pthread_mutex_lock(&lock_dir);

    bool result = false;

    try {
        boost::posix_time::ptime ptime = getLocalTime();

        boost::format output_fmt("%s/%04u-%04u-%02u-%02u_%02u_%s%s");
        std::stringstream fmt;
        // force check sync time
        setDatetimeSync();
        fmt << output_fmt
            % getPath()
            % getLastDirIndex()
            % ptime.date().year()
            % ptime.date().month().as_number()
            % ptime.date().day().as_number()
            % ptime.time_of_day().hours()
            % getTimeZone()
            % (getDatetimeSync() ? "" : "_UNSET");

        // check if dir not exists
        if (pathIsDir(fmt.str())) {
            __current_log_dir = fmt.str();
        }
        else
        {
            // increase index
            setLastDirIndex(getLastDirIndex() + 1);
            // update dir name
            fmt.str( std::string() );
            fmt.clear();
            // force check sync time
            setDatetimeSync();
            fmt << output_fmt
                % getPath()
                % getLastDirIndex()
                % ptime.date().year()
                % ptime.date().month().as_number()
                % ptime.date().day().as_number()
                % ptime.time_of_day().hours()
                % getTimeZone()
                % (getDatetimeSync() ? "" : "_UNSET");
            __current_log_dir = fmt.str();

            result = mkdir(getCurrentLogDir().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
            if (result) {
                writeInfo("TimeFolder: Current Log Dir = %s", getCurrentLogDir().c_str());
                pthread_mutex_unlock(&lock_file);
                setCurrentLogFile();
                pthread_mutex_lock(&lock_file);
            }
            else
                writeError("TimeFolder: Can't create Log Dir = %s", getCurrentLogDir().c_str());
        }
    } catch(...) { 
    }

    pthread_mutex_unlock(&lock_dir);
    pthread_mutex_unlock(&lock_file);

    return result;
}

std::string LogTypeTimeFolder::getCurrentLogDir() const
{
    return __current_log_dir;
}

void LogTypeTimeFolder::setCurrentLogFile()
{
    pthread_mutex_lock(&lock_file);

    try {
        boost::posix_time::ptime ptime = getLocalTime();

        boost::format output_fmt("%s/%02u-%02u-%02u.csv");
        std::stringstream fmt;
        fmt << output_fmt
            % getCurrentLogDir()
            % ptime.time_of_day().hours()
            % ptime.time_of_day().minutes()
            % ptime.time_of_day().seconds();
        __current_log_file = fmt.str();
        writeInfo("TimeFolder: Current Log File = %s", getCurrentLogFile().c_str());
        if ( __csvWorker->setNewFile(getCurrentLogFile()) )
            writeDebug("TimeFolder: New Log File open success");
        else
            writeError("TimeFolder: Can't open New Log File");
    } catch(...) { 
    }

    pthread_mutex_unlock(&lock_file);
}

std::string LogTypeTimeFolder::getCurrentLogFile() const
{
    return __current_log_file;
}

void LogTypeTimeFolder::reindex()
{
    pthread_mutex_lock(&lock_file);
    pthread_mutex_lock(&lock_dir);

    try {

        /* Dir reindex */
        {
            boost::optional<DirInfoVec> dirInfo = getDirInfo(getPath(), false);

            if (!dirInfo) {
                writeError("TimeFolder: Can't get info of log dir");
                pthread_mutex_unlock(&lock_dir);
                pthread_mutex_unlock(&lock_file);
                return;
            }

            DirInfoVec dirInfoVec = dirInfo.get();
            const uint32_t count = dirInfoVec.size();

            char current_index[5] = "0000";
            setLastDirIndex(0);

            for (uint32_t i = 0; i < count; ++i)
            {
                if (pathIsDir(dirInfoVec[i].first)) {
                    snprintf(current_index, 5, "%04u", i);

                    if ( rename(current_index, dirInfoVec[i].first) )
                        setLastDirIndex(std::atoi(current_index));
                    else
                        writeError("TimeFolder: Can't rename '%s'", dirInfoVec[i].first.c_str());
                }
            }

            writeDebug("TimeFolder: Last dir index = '%04u'", getLastDirIndex());

        } /* Dir reindex */

        pthread_mutex_unlock(&lock_dir);
        pthread_mutex_unlock(&lock_file);
        if (setCurrentLogDir()) {
            if ( __csvWorker->setNewFile(getCurrentLogFile()) )
                writeDebug("TimeFolder: New Log File open success");
            else
                writeError("TimeFolder: Can't open New Log File");
        }
        pthread_mutex_lock(&lock_dir);
        pthread_mutex_lock(&lock_file);

    } catch(...) {
        
    }

    pthread_mutex_unlock(&lock_dir);
    pthread_mutex_unlock(&lock_file);
}

bool LogTypeTimeFolder::rename(
    const std::string& new_index,
    const std::string& old_filename) const
{
    boost::regex expression("^(\\/.*\\/)([0-9]{4}-)([0-9]{4}-[0-9]{2}-[0-9]{2}_[0-9]{2}_[A-Za-z]+(_UNSET)?)$", boost::regex::perl | boost::regex::icase);
    boost::cmatch matches;

    if (boost::regex_match(old_filename.c_str(), matches, expression)
        && matches[1].matched
        && matches[2].matched
        && matches[3].matched)
    {
        std::string match_parent(matches[1].first, matches[1].second);
        std::string match_name(matches[3].first, matches[3].second);

        std::string new_filename = match_parent + new_index + "-" + match_name;

        return (::rename(old_filename.c_str(), new_filename.c_str()) == 0);
    }
    return false;
}

void LogTypeTimeFolder::clean()
{
    pthread_mutex_lock(&lock_file);
    pthread_mutex_lock(&lock_dir);

    try {

        uint64_t totalSize = 0;

        boost::regex expression("^(\\/.*\\/)([0-9]{4}-)([0-9]{4}-[0-9]{2}-[0-9]{2}_[0-9]{2}_[A-Za-z]+(_UNSET)?)$", boost::regex::perl | boost::regex::icase);
        boost::cmatch matches;

        /* Dir clean */
        {
            boost::optional<DirInfoVec> dirInfo = getDirInfo(getPath(), false);

            if (!dirInfo) {
                writeError("TimeFolder: Can't get info of log dir");
                pthread_mutex_unlock(&lock_dir);
                pthread_mutex_unlock(&lock_file);
                return;
            }

            DirInfoVec dirInfoVec = dirInfo.get();
                            pthread_mutex_unlock(&lock_dir);
                            pthread_mutex_unlock(&lock_file);
            const uint32_t dirCount = dirInfoVec.size();

            boost::optional<DirInfoVec> fileInfo;
            DirInfoVec fileInfoVec;
            uint32_t fileCount;

            for (uint32_t i = 0; i < dirCount; ++i)
            {
                if (pathIsDir(dirInfoVec[i].first)
                    && boost::regex_match(dirInfoVec[i].first.c_str(), matches, expression)
                    && matches[1].matched
                    && matches[2].matched
                    && matches[3].matched)
                {
                    // valid dir
                    fileInfo = getDirInfo(dirInfoVec[i].first);
                    // skip wrong dir
                    if (!fileInfo) {
                        writeError("TimeFolder: Can't get info in log dir = %s", dirInfoVec[i].first.c_str());
                        continue;
                    }

                    fileInfoVec = fileInfo.get();
                    fileCount = fileInfoVec.size();

                    for (uint32_t j = 0; j < fileCount; ++j)
                    {
                        if (fileInfoVec[j].second != UINT64_MAX) {
                            // if file empty - remove
                            // if (fileInfoVec[j].second == 0) {
                            //     if (unlink(fileInfoVec[j].first.c_str()) != 0)
                            //         writeError("TimeFolder: Can't delete file = %s", fileInfoVec[j].first.c_str());
                            // } else {
                            totalSize += fileInfoVec[j].second;
                            // }
                        }
                        else
                            writeError("TimeFolder: Can't stat %s", fileInfoVec[j].first.c_str());
                    }

                    // remove empty dir
                    if (fileCount == 0) {
                        if (rmdir(dirInfoVec[i].first.c_str()) != 0)
                            writeError("TimeFolder: Can't delete dir = %s", dirInfoVec[i].first.c_str());
                    }

                // clean wrong dir and files
                } else {

                    writeDebug("TimeFolder: Wrong file/dir in log dir = %s", dirInfoVec[i].first.c_str());

                    // clean dir recursive
                    if (pathIsDir(dirInfoVec[i].first)) {
                        fileInfo = getDirInfo(dirInfoVec[i].first);
                        // skip wrong dir
                        if (!fileInfo) {
                            writeError("TimeFolder: Can't get info in log dir = %s", dirInfoVec[i].first.c_str());
                            continue;
                        }

                        fileInfoVec = fileInfo.get();
                        fileCount = fileInfoVec.size();

                        // backward clean
                        for (int32_t j = fileCount - 1; j >= 0; --j)
                            if (unlink(fileInfoVec[j].first.c_str()) != 0)
                                writeError("TimeFolder: Can't delete file = %s", fileInfoVec[j].first.c_str());

                        if (rmdir(dirInfoVec[i].first.c_str()) != 0)
                            writeError("TimeFolder: Can't delete dir = %s", dirInfoVec[i].first.c_str());

                    } else {
                        if (unlink(dirInfoVec[i].first.c_str()) != 0)
                            writeError("TimeFolder: Can't delete file = %s", dirInfoVec[i].first.c_str());
                    }
                }
            }

        } /* Dir clean */

        uint64_t partitionFreeSpace = getFreeSpace(getPath());

        // out of limit
        if ( (totalSize > getMaxSize())
           || (partitionFreeSpace < getMinFree()))
        {
            boost::optional<DirInfoVec> fileInfo = getDirInfo(getPath());

            if (!fileInfo) {
                writeError("TimeFolder: Can't get info of log dir");
                pthread_mutex_unlock(&lock_dir);
                pthread_mutex_unlock(&lock_file);
                return;
            }

            DirInfoVec fileInfoVec = fileInfo.get();
            const uint32_t fileCount = fileInfoVec.size();

            for (uint32_t i = 0; i < fileCount; ++i)
            {
                if (fileInfoVec[i].second != UINT64_MAX) {
                    if (!pathIsDir(fileInfoVec[i].first.c_str())) {
                        if (unlink(fileInfoVec[i].first.c_str()) == 0) {
                            // control overflow, all file was deleted
                            totalSize -= fileInfoVec[i].second;
                            partitionFreeSpace += fileInfoVec[i].second;
                            if ( (totalSize <= getMaxSize())
                                && (partitionFreeSpace >= getMinFree())) {
                                pthread_mutex_unlock(&lock_dir);
                                pthread_mutex_unlock(&lock_file);
                                return;
                            }
                        }
                        else
                            writeError("TimeFolder: Can't delete file = %s", fileInfoVec[i].first.c_str());
                    }
                }
                else
                    writeError("TimeFolder: Can't stat %s", fileInfoVec[i].first.c_str());
            }

        }

    } catch(...) {
        
    }

    pthread_mutex_unlock(&lock_dir);
    pthread_mutex_unlock(&lock_file);
}

bool LogTypeTimeFolder::validate()
{
    return parsePath()
        && parseCreateDirInterval()
        && parseCreateFileInterval()
        && parseMaxSize()
        && parseMinFree()
        && parseFreqFreeSize()
        && parseDelimeter();
}


#define PTHREAD_CREATE(NAME, DESC) \
    if (pthread_create(&pth_ ## NAME, NULL, thread_ ## NAME, (void *)this) != 0) \
        writeError("TimeFolder: Can't create thread " #DESC); \
    else \
        writeDebug("TimeFolder: Create thread " #DESC);

#define PTHREAD_CANCEL(NAME, DESC) \
    if (pthread_cancel(pth_ ## NAME) != 0) \
        writeError("TimeFolder: thread " #DESC " cancel fail");

#define PTHREAD_JOIN(NAME, DESC) \
    { \
        void *res; \
        if (pthread_join(pth_ ## NAME, &res) != 0) \
            writeError("TimeFolder: thread " #DESC " join fail"); \
        else { \
            if (res == PTHREAD_CANCELED) \
                writeDebug("TimeFolder: thread " #DESC " canceled"); \
            else \
                writeError("TimeFolder: thread " #DESC " wasn't canceled"); \
        } \
    }

void LogTypeTimeFolder::run()
{
    setDatetimeSync();

    writeInfo("TimeFolder: Date Time is sync = %s", getDatetimeSync() ? "true" : "false");

    writeInfo("TimeFolder: Free space for Log Dir (root) = %s", 
        size2human(getFreeSpace(getPath()), true).c_str());

    writeInfo("TimeFolder: Free space for Log Dir (userspace) = %s", 
        size2human(getFreeSpaceNonRoot(getPath()), true).c_str());

    __csvWorker->setNewDelimeter(getDelimeter());

    PTHREAD_CREATE(udp_send, "UDP Sender");
    PTHREAD_CREATE(udp_recv, "UDP Receiver");

    stopThread();

    // wait every 1s terminate
    while (!isTerminate()) {
        usleep(1000 * 1000);
    }

    PTHREAD_CANCEL(udp_send, "UDP Sender");
    PTHREAD_CANCEL(udp_recv, "UDP Receiver");
    PTHREAD_JOIN(udp_send, "UDP Sender");
    PTHREAD_JOIN(udp_recv, "UDP Receiver");

    // thread terminated
    if (isThreadRun()) {
        PTHREAD_CANCEL(gc, "GC");
        PTHREAD_CANCEL(create_dir, "create dir");
        PTHREAD_CANCEL(create_file, "create file");

        PTHREAD_JOIN(gc, "GC");
        PTHREAD_JOIN(create_dir, "create dir");
        PTHREAD_JOIN(create_file, "create file");
    }
}

void LogTypeTimeFolder::callbackSWA(const uint8_t typeId, const uint16_t dataLength, const uint8_t* data)
{
    writeDebug("Type: %u, Len = %u, Data = ", typeId, dataLength);
    for (int i = 0; i < dataLength; ++i)
        writeDebug("%02X", data[i]);

    switch(typeId)
    {
        // Hello package
        case 0:
            writeDebug("Get valid hello package\n");

            // run threads if get Hello package first time
            if (!isThreadRun()) {

                clean();
                clean();
                reindex();
                setCurrentLogDir();
                setCurrentLogFile();
                __csvWorker->setNewFile(getCurrentLogFile());

                setSWAHelloReceive();
                startThread();

                PTHREAD_CREATE(gc, "GC");
                PTHREAD_CREATE(create_dir, "create dir");
                PTHREAD_CREATE(create_file, "create file");
            }

            break;

        // CSV package
        case 1:
            writeDebug("Get valid csv package\n");

            if ((dataLength == 1) && (data[0] == 0xFF))
            {
                writeDebug("Save csv data\n");
                // reallocate vector, set maxLength
                if (__csvSetMaxIndex)
                    __csvSetMaxIndex = false;
                try {
                    pthread_mutex_lock(&lock_file);
                    __csvWorker->save();
                } catch(...) {
                }
                pthread_mutex_unlock(&lock_file);
            }
            else
            {
                int index = data[0];
                if (__csvSetMaxIndex)
                    __csvWorker->setMaxSize(index + 1);
                // skip after out of range index inside
                __csvWorker->addData(index, data + 1, dataLength - 2);
            }

            break;

        // Invalid package
        default:
            writeDebug("Invalid package\n");
            break;
    }
}

#undef PTHREAD_CREATE
#undef PTHREAD_CANCEL
#undef PTHREAD_JOIN