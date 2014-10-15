#ifndef LOGGER_CSV_CSV_WORKER_HPP
#define LOGGER_CSV_CSV_WORKER_HPP

/* c headers */
#include <stdint.h>

/* c++ headers */
#include <fstream>
#include <string>
#include <vector>

/* boost headers */
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

class CSVWorker
{
private:
    std::ofstream               __file;
    char                        __delimiter;
    uint32_t                    __csvDataSize;
    bool                        __csvDataEmpty;
    bool                        __escapeString;
    std::vector< boost::array<uint8_t, 65535> > __csvData;

    std::string& prepareData(std::string&) const;

    bool isDataEmpty() const;
    void setDataEmpty();
    void setDataNotEmpty();

    uint32_t getDataSize() const;
    void setDataSize(const uint32_t);

    std::ofstream& getFile();
    bool fileIsOpen();
    void fileClose();

    char getDelimeter() const;

public:
    CSVWorker();
    CSVWorker(const std::string&, const char, const bool = true);
    ~CSVWorker();

    bool setFile(const std::string&);
    void setDelimeter(const char);
    void setMaxSize(const uint32_t);
    void setEscapeString(const bool);

    bool getEscapeString() const;

    void addData(const uint32_t, const uint8_t*, const uint16_t);
    bool save();
};

typedef boost::shared_ptr<CSVWorker> CSVWorkerPtr;

#endif /* LOGGER_CSV_CSV_WORKER_HPP */