#ifndef OS_UTILS_H
#define OS_UTILS_H

#ifdef _WIN32
#include "os_utils_win32.h"
#elif __linux__
#include "os_utils_linux.h"
#endif

#endif // OS_UTILS_H
