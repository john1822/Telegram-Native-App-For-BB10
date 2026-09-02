#include "Config.h"

namespace Telegram {

const char* const Config::DEFAULT_DC_IP = "91.108.56.165";
const char* const Config::API_HASH = "b18441a1ff607e10a989891a5462e627";
const char* const Config::DEVICE_MODEL = "BlackBerry 10";
const char* const Config::SYSTEM_VERSION = "QNX 6.6 / BB10";
const char* const Config::APP_VERSION = "1.0.0 (Native BB10 2026)";
const char* const Config::SYSTEM_LANG_CODE = "en";
const char* const Config::LANG_PACK = "";
const char* const Config::LANG_CODE = "en";

bool Config::getDcAddress(int dcId, QString& ipOut, int& portOut) {
    portOut = 443;
    switch (dcId) {
        case 1: ipOut = "149.154.175.53"; return true;
        case 2: ipOut = "149.154.167.50"; return true;
        case 3: ipOut = "149.154.175.100"; return true;
        case 4: ipOut = "149.154.167.91"; return true;
        case 5: ipOut = "91.108.56.165"; return true;
        default: return false;
    }
}

} // namespace Telegram
