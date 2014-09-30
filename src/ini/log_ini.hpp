#ifndef LOGGER_INI_LOG_INI_HPP
#define LOGGER_INI_LOG_INI_HPP

/* c headers */
#include <stdint.h>

/* c++ headers */
#include <string>

/* boost headers */
#include <boost/property_tree/ptree.hpp>

/* class headers */
#include "../syslog/log_syslog.hpp"

/* Class for different types of logging
    collect common settings */
class LogIni
{

private:
    boost::property_tree::ptree __iniTree;

    // isolate for reduce depend of boost ini_parse error
    // set error false if get value without error
    // otherway set error true
    bool        __error;
    std::string __errorText;

public:
    LogIni();
    virtual ~LogIni();

    bool iniParse(const std::string&);

    bool iniGetError() const;
    std::string iniGetErrorText() const;

    template <typename T>
    T iniGetValue(const std::string& section, const std::string& variable, T default_value) {
        __error = false;
        try {
            boost::optional<T> value = __iniTree.get_optional<T>(section + "." + variable);
            // if not found field, just return default value
            return !value ? default_value : value.get();
        }
        catch (const boost::property_tree::ptree_bad_data& error)
        {
            __error = true;
            __errorText = error.what();
        }
        catch (const boost::property_tree::ptree_bad_path& error)
        {
            __error = true;
            __errorText = error.what();
        }
        return default_value;
    }
};

#endif /* LOGGER_INI_LOG_INI_HPP */
