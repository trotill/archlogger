#ifndef LOGGER_INI_SETTINGS_HPP
#define LOGGER_INI_SETTINGS_HPP

/* c headers */
#include <stdint.h>

/* c++ headers */
#include <string>

namespace ini
{
    const static std::string SECTION                = "SWASettings";
    const static std::string VAR_SWA_STORAGETYPE    = "SWAStorageType";
    const static std::string VAR_SWA_IP             = "SWAIP";
    const static std::string VAR_SWA_REMOTEPORT     = "SWARemotePort";
    const static std::string VAR_SWA_SOURCEPORT     = "SWASourcePort";
    const static std::string VAR_SWA_UDPBUFFERSIZE  = "SWAUDPBufferSize";

    const static std::string TF_SECTION                      = "SWASettings";
    const static std::string VAR_TF_SWA_PATH                 = "tfSWAPath";
    const static std::string VAR_TF_SWA_CREATE_DIR_INTERVAL  = "tfSWACreateDirInterval";
    const static std::string VAR_TF_SWA_CREATE_FILE_INTERVAL = "tfSWACreateFileInterval";
    const static std::string VAR_TF_SWA_MAX_SIZE             = "tfSWAMaxSize";
    const static std::string VAR_TF_SWA_MIN_FREE             = "tfSWAMinFree";
    const static std::string VAR_TF_SWA_FREQ_FREE_SIZE       = "tfSWAFreqFreeSize";
    const static std::string VAR_TF_SWA_DELIMETER            = "tfSWADelimeter";

    /* default values */
    namespace settings {
        static const std::string configFile  = "/SWA.ini";
        static const uint8_t storageType     = 1;
        static const std::string ipAddress   = "127.0.0.1";
        static const uint16_t remotePort     = 9001;
        static const uint16_t sourcePort     = 0;
        static const uint64_t udpBufferSize  = 1 * 1024 * 1024;

        /* default values for TimeFolder */
        namespace tf {
            static const std::string path               = "/opt";
            static const uint64_t createDirInterval     = 60 * 60 * 1000;
            static const uint64_t createFileInterval    = 15 * 60 * 1000;
            static const uint64_t maxSize               = 100 * 1024 * 1024;
            static const uint64_t minFree               = 10 * 1024 * 1024;
            static const uint64_t freqFreeSize          = 1000;
            static const char delimeter                 = ',';
        }
    }
}

#endif /* LOGGER_INI_SETTINGS_HPP */