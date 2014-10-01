#ifndef LOGGER_LOGTYPE_HPP
#define LOGGER_LOGTYPE_HPP

/* c headers */
#include <stdint.h>

/* c++ headers */
#include <string>

/* boost headers */
#include <boost/array.hpp>
#include <boost/asio.hpp>

/* class headers */
#include "helper/log_helper.hpp"
#include "syslog/log_syslog.hpp"
#include "ini/log_ini.hpp"
#include "swa/swa_parser.hpp"

/* Class for different types of logging
    collect common settings */
class LogType:
    public virtual LogHelper,
    public virtual LogSyslog,
    public virtual LogIni
{

public:
    enum log_storage_types {
        LOG_STORAGETYPE_UNKNOWN    = 0,
        LOG_STORAGETYPE_TIMEFOLDER = 1,
        // used only for control types
        LOG_STORAGETYPE_LAST       = LOG_STORAGETYPE_TIMEFOLDER
    };

    /* INI-config */
private:
    std::string __configFile;
    uint8_t     __storageType;
    std::string __ipAddress;
    uint16_t    __remotePort;
    uint16_t    __sourcePort;

private:
    bool parseStorageType();
    bool parseIpAddress();
    bool parseRemotePort();
    bool parseSourcePort();

public:
    void setConfigFile(const std::string&);
    void setStorageType(const uint8_t);
    void setIpAddress(const std::string&);
    void setRemotePort(const uint16_t);
    void setSourcePort(const uint16_t);

public:
    std::string getConfigFile() const;
    uint8_t     getStorageType() const;
    std::string getIpAddress() const;
    uint16_t    getRemotePort() const;
    uint16_t    getSourcePort() const;

    std::string getStorageTypeAsString() const;
    /* INI-config */

    /* SWA */
private:
    SWAParserPtr __swaParser;
    bool        __swaHelloReceive;

public:
    bool parseSWA(const uint8_t*) const;

    void setSWAHelloReceive();
    bool getSWAHelloReceive() const;

protected:
    virtual void callbackSWA(const uint8_t, const uint16_t, const uint8_t*);
    /* SWA */

    /* UDP */ 
private:
    boost::asio::io_service        __io_service;
    boost::asio::ip::udp::endpoint *__udp_remote_endpoint;
    boost::asio::ip::udp::socket   *__udp_socket;

protected:
    void closeUdp();

public:
    boost::asio::ip::udp::socket*   getUdpSocket();
    boost::asio::ip::udp::endpoint* getUdpRemoteEndpoint();
    /* UDP */

    /* Thread */
private:
    bool        __threadRun;

public:
    bool isThreadRun() const;

    void startThread();
    void stopThread();
    /* Thread */

    /* Terminate */
private:
    bool        __terminate;

public:
    void terminate();
    bool isTerminate() const;
    /* Terminate */

public:
    LogType();
    LogType(LogType&);
    virtual ~LogType();

    virtual bool validate();
    virtual void run();
};

#endif /* LOGGER_LOGTYPE_HPP */
