#ifndef LOGGER_LOGTYPE_TIMEFOLDER_HPP
#define LOGGER_LOGTYPE_TIMEFOLDER_HPP

/* c headers */
#include <stdint.h>
#include <pthread.h>

/* c++ headers */
#include <string>

/* class headers */
#include "logtype.hpp"
#include "csv/csv_worker.hpp"

class LogTypeTimeFolder: public LogType
{
    /* INI-config */
private:
    std::string __path;
    uint64_t    __createDirInterval;
    uint64_t    __createFileInterval;
    uint64_t    __maxSize;
    uint64_t    __minFree;
    uint64_t    __freqFreeSize;
    char        __delimeter;

private:
    bool parsePath();
    bool parseCreateDirInterval();
    bool parseCreateFileInterval();
    bool parseMaxSize();
    bool parseMinFree();
    bool parseFreqFreeSize();
    bool parseDelimeter();

public:
    void setPath(const std::string&);
    void setCreateDirInterval(const uint64_t);
    void setCreateFileInterval(const uint64_t);
    void setMaxSize(const uint64_t);
    void setMinFree(const uint64_t);
    void setFreqFreeSize(const uint64_t);
    void setDelimeter(const char);

public:
    std::string getPath() const;
    uint64_t    getCreateDirInterval() const;
    uint64_t    getCreateFileInterval() const;
    uint64_t    getMaxSize() const;
    uint64_t    getMinFree() const;
    uint64_t    getFreqFreeSize() const;
    char        getDelimeter() const;

public:
    std::string getCreateDirIntervalAsString() const;
    std::string getCreateFileIntervalAsString() const;
    std::string getMaxSizeAsString() const;
    std::string getMinFreeAsString() const;
    std::string getFreqFreeSizeAsString() const;
    /* INI-config */

    /* Logic of log */
private:
    pthread_mutex_t lock_file;
    pthread_mutex_t lock_dir;

    bool        __datetimeSync;
    uint16_t    __last_dir_index;
    std::string __current_log_dir;
    std::string __current_log_file;

    void setDatetimeSync();
    bool getDatetimeSync() const;

    void setLastDirIndex(const uint16_t);
    uint16_t getLastDirIndex() const;

    std::string getCurrentLogDir() const;
    std::string getCurrentLogFile() const;

public:
    bool setCurrentLogDir();
    void setCurrentLogFile();

private:
    bool rename(const std::string&, const std::string&) const;

public:
    void reindex();
    void clean();
    /* Logic of log */

    /* CSV */
private:
    CSVWorkerPtr    __csvWorker;
    bool            __csvSetMaxIndex;
    /* CSV */

    /* Thread */
private:
    pthread_t pth_gc;
    pthread_t pth_create_dir;
    pthread_t pth_create_file;
    pthread_t pth_udp_send;
    pthread_t pth_udp_recv;
    /* Thread */

    /* SWA */
protected:
    virtual void callbackSWA(const uint8_t, const uint16_t, const uint8_t*);
    /* SWA */

public:
    LogTypeTimeFolder(const LogType&);
    virtual ~LogTypeTimeFolder();

    virtual bool validate();
    virtual void run();
};

#endif /* LOGGER_LOGTYPE_TIMEFOLDER_HPP */
