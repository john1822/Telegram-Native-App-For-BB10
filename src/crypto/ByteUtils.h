#ifndef TELEGRAM_BYTE_UTILS_H
#define TELEGRAM_BYTE_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

namespace Telegram {

template<typename T>
inline T* vecData(std::vector<T>& v) {
    return v.empty() ? NULL : &v[0];
}

template<typename T>
inline const T* vecData(const std::vector<T>& v) {
    return v.empty() ? NULL : &v[0];
}

} // namespace Telegram

#endif // TELEGRAM_BYTE_UTILS_H
