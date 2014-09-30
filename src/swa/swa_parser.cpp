
/* class headers */
#include "swa_parser.hpp"

SWAParser::SWAParser(const SWACallback& callback)
{
    try {

        __callback.clear();
        __callback = callback;

    } catch (const std::exception&) {
        __callback.clear();
    }
}

SWAParser::~SWAParser()
{

}

bool SWAParser::validate(
    const uint8_t typeId,
    const uint16_t dataLength,
    const uint8_t* data) const
{
    switch (typeId)
    {
        // SWA Hello
        case 0:
            return ((dataLength == 6)
                     && (data[0] == 0x68)
                     && (data[1] == 0x65)
                     && (data[2] == 0x6C)
                     && (data[3] == 0x6C)
                     && (data[4] == 0x6F)
                     && (data[5] == 0x00));

        // SWA CSV
        case 1:
            return ((dataLength == 1)
                    && (data[0] == 255))
                    || 
                    ((dataLength > 1)
                    /* && (data[0] >= 0) */ 
                    && (data[0] <= 199)
                    && (data[dataLength - 1] == 0x00));

        // wrong type
        default:
            return false;
    }
}

bool SWAParser::parseData(const uint8_t* input)
{
    const uint8_t typeId = input[0];
    const uint16_t dataLength = ((uint16_t)input[2] << 8) | (uint16_t)input[3];
    const uint8_t* data = (dataLength > 0) ? input + 4 : NULL;

    if (validate(typeId, dataLength, data) && __callback) {
        __callback(typeId, dataLength, data);
        return true;
    }

    return false;
}