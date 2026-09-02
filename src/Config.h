#ifndef TELEGRAM_CONFIG_H
#define TELEGRAM_CONFIG_H

#include <stdint.h>
#include <stddef.h>
#include <QString>

namespace Telegram {

struct Config {
    static const int DEFAULT_DC_ID = 5;
    static const char* const DEFAULT_DC_IP;
    static const int DEFAULT_DC_PORT = 443;
    
    static bool getDcAddress(int dcId, QString& ipOut, int& portOut);
    
    // Official Telegram client API credentials (configurable)
    static const int32_t API_ID = 2040;
    static const char* const API_HASH;
    
    static const char* const DEVICE_MODEL;
    static const char* const SYSTEM_VERSION;
    static const char* const APP_VERSION;
    static const char* const SYSTEM_LANG_CODE;
    static const char* const LANG_PACK;
    static const char* const LANG_CODE;
};

} // namespace Telegram

#endif // TELEGRAM_CONFIG_H
