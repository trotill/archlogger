#ifndef LOGGER_CSV_CSV_WORKER_HPP
#define LOGGER_CSV_CSV_WORKER_HPP

/* c headers */
#include <stdint.h>

/* c++ headers */
#include <fstream>
#include <string>
#include <vector>

/* boost headers */
#include <boost/shared_ptr.hpp>

class CSVWorker
{
private:
    std::ofstream				__file;
    char 						__delimiter;
    uint32_t                    __csvDataSize;
    bool                        __csvDataEmpty;
    std::vector<std::string>    __csvData;

    std::string& prepareData(std::string&) const;

    bool isDataEmpty() const;
    void setDataEmpty();
    void setDataNotEmpty();

    uint32_t getDataSize() const;
    void setDataSize(const uint32_t);

public:
    CSVWorker();
    CSVWorker(const std::string&, const char);
    ~CSVWorker();

    bool setNewFile(const std::string&);
    void setNewDelimeter(const char);
    bool save();
    void setMaxSize(const uint32_t);
    void addData(const uint32_t, const uint8_t*, const uint16_t);
};

typedef boost::shared_ptr<CSVWorker> CSVWorkerPtr;

#endif /* LOGGER_CSV_CSV_WORKER_HPP */