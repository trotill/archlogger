
/* boost headers */
#include <boost/algorithm/string/replace.hpp>

/* class headers */
#include "csv_worker.hpp"

CSVWorker::CSVWorker() :
    __delimiter(','),
    __csvDataSize(1),
    __csvData(1)
{
    setDataEmpty();
    setEscapeString(true);
}

CSVWorker::CSVWorker(
    const std::string& filename,
    const char delimiter,
    const bool escapeString)
    :
    __csvDataSize(1),
    __csvData(1)
{
    setDataEmpty();
    setEscapeString(escapeString);
    setDelimeter(delimiter);
    getFile().open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
}

CSVWorker::~CSVWorker()
{
    fileClose();
}

bool CSVWorker::setFile(const std::string& filename)
{
    fileClose();
    getFile().open (filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    return fileIsOpen();
}

void CSVWorker::setMaxSize(const uint32_t size)
{
    if (size > getDataSize())
    {
        setDataSize(size);
        __csvData.resize(size);
    }
}

void CSVWorker::setDelimeter(const char delimiter)
{
    __delimiter = delimiter;
}

void CSVWorker::setEscapeString(const bool escapeString)
{
    __escapeString = escapeString;
}

void CSVWorker::addData(const uint32_t index, const uint8_t* data, const uint16_t dataLength)
{
    // ignore out of range
    if (index >= getDataSize())
        return;

    memcpy(&__csvData[index], data, dataLength);

    setDataNotEmpty();
}

std::string& CSVWorker::prepareData(std::string& csvData) const
{
    // empty input string
    // OR not need prepare escape
    if (csvData.empty() || !getEscapeString())
        return csvData;

    boost::replace_all(csvData, "\"", "\"\"");

    if ((csvData.find(' ') != std::string::npos) ||
        (csvData.find('\'') != std::string::npos) ||
        (csvData.find('"') != std::string::npos) ||
        (csvData.find(',') != std::string::npos) ||
        (csvData.find(';') != std::string::npos) ||
        (csvData.find('\r') != std::string::npos) ||
        (csvData.find('\n') != std::string::npos)) {
        csvData = '"' + csvData + '"';
    }
    return csvData;
}

bool CSVWorker::isDataEmpty() const
{
    return __csvDataEmpty;
}

void CSVWorker::setDataEmpty()
{
    __csvDataEmpty = true;
}

void CSVWorker::setDataNotEmpty()
{
    __csvDataEmpty = false;
}

uint32_t CSVWorker::getDataSize() const
{
    return __csvDataSize;
}

void CSVWorker::setDataSize(const uint32_t size)
{
    __csvDataSize = size;
}

std::ofstream& CSVWorker::getFile()
{
    return __file;
}

bool CSVWorker::fileIsOpen()
{
    return getFile().is_open();
}

void CSVWorker::fileClose()
{
    if (fileIsOpen()) {
        getFile().flush();
        getFile().close();
    }
}

char CSVWorker::getDelimeter() const
{
    return __delimiter;
}

bool CSVWorker::getEscapeString() const
{
    return __escapeString;
}

bool CSVWorker::save()
{
    // skip empty data
    if (isDataEmpty())
        return false;

    if (!getFile().is_open())
        return false;

    // don't write last element

    char arr[65535];
    std::string str;

    uint32_t size = getDataSize() - 1;
    for (size_t i = 0; i < size; ++i) {
        // empty string
        if (__csvData[i][0] == 0x00) {
            getFile() << getDelimeter();
        } else {
            memcpy(&arr, &(__csvData[i]), 65535);
            // skip null bytes in the end of string
            str = std::string(arr, strlen(arr));
            getFile() << prepareData(str) << getDelimeter();
        }
    }

    // empty string
    if (__csvData[size][0] == 0x00) {
        getFile() << "\r\n";
    } else {
        memcpy(&arr, &(__csvData[size]), 65535);
        // skip null bytes in the end of string
        str = std::string(arr, strlen(arr));
        getFile() << prepareData(str) << "\r\n";
    }

    getFile().flush();

    __csvData.resize(getDataSize());

    for (size_t i = 0; i < size; ++i)
        memset(&__csvData[i], 0, 65535);

    setDataEmpty();

    return true;
}
