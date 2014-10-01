
/* c++ headers */
#include <sstream>
#include <stdexcept>

/* class headers */
#include "logtype.hpp"
#include "ini/settings.hpp"

/* boost headers */
#include <boost/make_shared.hpp>

/* config header */
#include "../config.h"

LogType::LogType()
{
    __configFile = ini::settings::configFile;
    __storageType = ini::settings::storageType;
    __ipAddress = ini::settings::ipAddress;
    __remotePort = ini::settings::remotePort;
    __sourcePort = ini::settings::sourcePort;

    __swaParser = boost::make_shared<SWAParser>(
        boost::bind(&LogType::callbackSWA, this, _1, _2, _3));
    if (!__swaParser) {
        writeError ("SWA Parser not allocated");
        // impossible, but if something corrupt memory.
        throw std::runtime_error( "SWA Parser not allocated" );
    }

    __threadRun = false;
    __swaHelloReceive = false;
    __terminate = false;

    __udp_socket = NULL;
    __udp_remote_endpoint = NULL;
}

LogType::LogType(LogType& copy):
    LogSyslog(false)
{
    copy.closeUdp();

    setVerbose(copy.getVerbose());
    setConfigFile(copy.getConfigFile());
    setStorageType(copy.getStorageType());
    setIpAddress(copy.getIpAddress());
    setRemotePort(copy.getRemotePort());
    setSourcePort(copy.getSourcePort());

    __swaParser = boost::make_shared<SWAParser>(
        boost::bind(&LogType::callbackSWA, this, _1, _2, _3));
    if (!__swaParser) {
        writeError ("SWA Parser not allocated");
        // impossible, but if something corrupt memory.
        throw std::runtime_error( "Wrong log priority." );
    }

    // check if config file exists
    if( !iniParse(getConfigFile()) )
    {
        writeError ("Can't parse config FILE = %s", getConfigFile().c_str());
        // impossible, but if something corrupt memory.
        throw std::runtime_error( "Wrong log priority." );
    }

    __threadRun = false;
    __swaHelloReceive = false;
    __terminate = false;

    __udp_socket = new boost::asio::ip::udp::socket(
        __io_service,
        boost::asio::ip::udp::endpoint(
            boost::asio::ip::address::from_string(copy.getIpAddress()),
            copy.getSourcePort()));
    __udp_remote_endpoint = new boost::asio::ip::udp::endpoint(
        boost::asio::ip::udp::endpoint(
            boost::asio::ip::address::from_string(copy.getIpAddress()),
            copy.getRemotePort()));
}

LogType::~LogType()
{
    __swaParser.reset();
    closeUdp();
}

void LogType::setConfigFile(const std::string& configFile)
{
    __configFile = configFile;
}

void LogType::setStorageType(const uint8_t storageType)
{
    __storageType = storageType;
}

void LogType::setIpAddress(const std::string& ipAddress)
{
    __ipAddress = ipAddress;
}

void LogType::setRemotePort(const uint16_t remotePort)
{
    __remotePort = remotePort;
}

void LogType::setSourcePort(const uint16_t sourcePort)
{
    __sourcePort = sourcePort;
}

std::string LogType::getConfigFile() const
{
    return __configFile;
}

uint8_t LogType::getStorageType() const
{
    return __storageType;
}

std::string LogType::getIpAddress() const
{
    return __ipAddress;
}

uint16_t LogType::getRemotePort() const
{
    return __remotePort;    
}

uint16_t LogType::getSourcePort() const
{
    return __sourcePort;
}

std::string LogType::getStorageTypeAsString() const
{
    switch(getStorageType())
    {
    case LOG_STORAGETYPE_TIMEFOLDER:
        return "TimeFolder";
    default:
        return "Unknown";
    }
}

bool LogType::parseStorageType()
{
    setStorageType( iniGetValue( ini::SECTION,
        ini::VAR_SWA_STORAGETYPE, ini::settings::storageType ));

    if (iniGetError())
    {
        writeError( "Can't get 'Storage Type'" );
        return false;
    }

    if ((getStorageType() == LOG_STORAGETYPE_UNKNOWN)
        || (getStorageType() > LOG_STORAGETYPE_LAST))
    {
        writeError( "'Storage Type' invalid" );
        return false;
    }

    writeInfo ( "Storage Type = %d (%s)", getStorageType(), getStorageTypeAsString().c_str());

    return true;
}

bool LogType::parseIpAddress()
{
    setIpAddress ( iniGetValue( ini::SECTION, 
        ini::VAR_SWA_IP, ini::settings::ipAddress ) );

    if (iniGetError())
    {
        writeError( "Can't get 'IP Address'" );
        return false;
    }

    if( getIpAddress().empty() )
    {
        writeError( "'IP Address' invalid" );
        return false;
    }

    writeInfo ( "IP Address = %s", getIpAddress().c_str());

    return true;
}

bool LogType::parseRemotePort()
{
    setRemotePort( iniGetValue( ini::SECTION,
        ini::VAR_SWA_REMOTEPORT, ini::settings::remotePort ));

    if (iniGetError())
    {
        writeError( "Can't get 'Remote Port'" );
        return false;
    }

    if ((getRemotePort() == 0) 
        || (getRemotePort() == UINT16_MAX))
    {
        writeError( "'Remote Port' invalid" );
        return false;
    }

    writeInfo ( "Remote Port = %d", getRemotePort());

    return true;
}

bool LogType::parseSourcePort()
{
    setSourcePort( iniGetValue( ini::SECTION,
        ini::VAR_SWA_SOURCEPORT, ini::settings::sourcePort ));

    if (iniGetError())
    {
        writeError( "Can't get 'Source Port'" );
        return false;
    }

    if (getSourcePort() == UINT16_MAX)
    {
        writeError( "'Source Port' invalid" );
        return false;
    }

    writeInfo ( "Source Port = %d", getSourcePort());

    return true;
}

bool LogType::parseSWA(const uint8_t* input) const
{
    return __swaParser.get()->parseData(input);
}

void LogType::callbackSWA(const uint8_t typeId, const uint16_t dataLength, const uint8_t* data)
{
    // nothing to do
}

bool LogType::validate()
{
    writeInfo ( "config FILE = %s", getConfigFile().c_str());

    // check if config file exists
    if( !fileExists(getConfigFile()) )
    {
        writeError ("Can't find config FILE = %s", getConfigFile().c_str());
        return false;
    }

    // check if config file can read
    if( !fileReadable(getConfigFile()) )
    {
        writeError ("Can't read from config FILE = %s", getConfigFile().c_str());
        return false;
    }

    // check if config file exists
    if( !iniParse(getConfigFile()) )
    {
        writeError ("Can't parse config FILE = %s", getConfigFile().c_str());
        return false;
    }

    bool result = parseStorageType()
                    && parseIpAddress()
                    && parseRemotePort()
                    && parseSourcePort();

    if (result) {
        __udp_socket = new boost::asio::ip::udp::socket(
            __io_service,
            boost::asio::ip::udp::endpoint(
                boost::asio::ip::address::from_string(getIpAddress()),
                getSourcePort()));
        __udp_remote_endpoint = new boost::asio::ip::udp::endpoint(
            boost::asio::ip::udp::endpoint(
                boost::asio::ip::address::from_string(getIpAddress()),
                getRemotePort()));
    }

    return result;
}

void LogType::run()
{
    // nothing to do
}

void LogType::startThread()
{
    __threadRun = true;
}

void LogType::stopThread()
{
    __threadRun = false;
}

bool LogType::isThreadRun() const
{
    return __threadRun;
}

void LogType::setSWAHelloReceive()
{
    __swaHelloReceive = true;
}

bool LogType::getSWAHelloReceive() const
{
    return __swaHelloReceive;
}

void LogType::terminate()
{
    __terminate = true;
}

bool LogType::isTerminate() const
{
    return __terminate;
}

boost::asio::ip::udp::socket*
LogType::getUdpSocket()
{
    return __udp_socket;
}

boost::asio::ip::udp::endpoint*
LogType::getUdpRemoteEndpoint()
{
    return __udp_remote_endpoint;
}

void LogType::closeUdp()
{
    if (__udp_socket != NULL) {
        __udp_socket->cancel();
        __udp_socket->close();
        delete __udp_socket;
        __udp_socket = NULL;
    }
    if (__udp_remote_endpoint != NULL) {
        delete __udp_remote_endpoint;
        __udp_remote_endpoint = NULL;
    }

    __io_service.stop();
    __io_service.reset();
}