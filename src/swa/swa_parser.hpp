#ifndef LOGGER_SWA_SWA_PARSER_HPP
#define LOGGER_SWA_SWA_PARSER_HPP

/* c headers */
#include <stdint.h>

/* boost headers */
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

// callback function prototype void f(typeId, data len, data)
typedef boost::function <void (const uint8_t, const uint16_t, const uint8_t*)> SWACallback;

class SWAParser
{
private:
    SWACallback __callback;

    bool validate(const uint8_t, const uint16_t, const uint8_t*) const;

public:
    SWAParser(const SWACallback&);
    ~SWAParser();

    bool parseData(const uint8_t*);
};

typedef boost::shared_ptr<SWAParser> SWAParserPtr;

#endif /* LOGGER_SWA_SWA_PARSER_HPP */