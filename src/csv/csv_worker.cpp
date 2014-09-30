
/* boost headers */
#include <boost/algorithm/string/replace.hpp>

/* class headers */
#include "csv_worker.hpp"

CSVWorker::CSVWorker() :
    __delimiter(','),
    __csvDataSize(1),
    __csvData(1)
{
    __csvDataEmpty = true;
}

CSVWorker::CSVWorker(const std::string& filename, const char delimiter) :
    __csvData(1)
{
    __file.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    __delimiter = delimiter;
}

CSVWorker::~CSVWorker()
{
    if (__file.is_open()) {
        __file.flush();
        __file.close();
    }
}

bool CSVWorker::setNewFile(const std::string& filename)
{
    if (__file.is_open()) {
        __file.flush();
        __file.close();
    }

    __file.open (filename.c_str(), std::ofstream::out | std::ofstream::trunc);

    return (__file.is_open());
}

void CSVWorker::setMaxSize(const uint32_t size)
{
    if (size > getDataSize())
    {
        setDataSize(size);
        __csvData.resize(size);
    }
}

void CSVWorker::setNewDelimeter(const char delimiter)
{
    __delimiter = delimiter;
}

void CSVWorker::addData(const uint32_t index, const uint8_t* data, const uint16_t dataLength)
{
    // ignore out of range
    if (index >= __csvDataSize)
        return;

    const char* c_data = reinterpret_cast<const char*>(data);

    __csvData[index] = std::string(c_data, dataLength);

    setDataNotEmpty();
}

std::string& CSVWorker::prepareData(std::string& csvData) const
{
    if (csvData.empty())
        return csvData;

    boost::replace_all(csvData, "\"", "\"\"");

    if ((csvData.find(' ') != std::string::npos) ||
        (csvData.find('\'') != std::string::npos) ||
        (csvData.find('"') != std::string::npos) ||
        (csvData.find(',') != std::string::npos) ||
        (csvData.find(';') != std::string::npos) ||
        (csvData.find('\r') != std::string::npos) ||
        (csvData.find('\n') != std::string::npos)) {
        csvData.insert(0, "\"");
        csvData += '"';
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

bool CSVWorker::save()
{
    // skip empty data
    if (isDataEmpty())
        return false;

    if (!__file.is_open())
        return false;

    // don't write last element
    uint32_t size = getDataSize() - 1;
    for (size_t i = 0; i < size; ++i) {
        __file << prepareData(__csvData[i]) << __delimiter;
    }

    __file << prepareData(__csvData[size]) << "\r\n";
    __file.flush();

    __csvData.clear();
    __csvData.resize(getDataSize());
    setDataEmpty();

    return true;
}
