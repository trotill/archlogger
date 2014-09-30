
/* boost headers */
#include <boost/property_tree/ini_parser.hpp>

/* class headers */
#include "log_ini.hpp"

LogIni::LogIni()
{
    // nothing to do
}

LogIni::~LogIni()
{
    // nothing to do
}

bool LogIni::iniParse(const std::string& filename)
{
    try {
        boost::property_tree::ini_parser::read_ini( filename, __iniTree);
        return true;
    } catch (...) {
    }
    return false;
}

bool LogIni::iniGetError() const
{
    return __error;
}

std::string LogIni::iniGetErrorText() const
{
    return __errorText;
}