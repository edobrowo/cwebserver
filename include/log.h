#ifndef LOG
#define LOG

#include <stdarg.h>

#define INFO_ENABLED 1
#define MORE_ENABLED 1
#define ERRR_ENABLED 1

typedef enum {
    INFO = 0,
    MORE,
    ERRR,
} log_msg_t;

// Logging util
void wslog(log_msg_t type, const char* format, ...);

#endif // LOG
