#ifndef LOG
#define LOG

#include <stdarg.h>

typedef enum {
    INFO = 0,
    WARN,
    ERRR,
} log_msg_t;

// Logging util
void wslog(log_msg_t type, const char* format, ...);

#endif // LOG
