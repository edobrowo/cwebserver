#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "log.h"

void wslog(log_msg_t type, const char* format, ...) {
    char msg[4096] = {0};

    switch (type) {
        case INFO: if (!INFO_ENABLED) return; break;
        case MORE: if (!MORE_ENABLED) return; break;
        case ERRR: if (!ERRR_ENABLED) return; break;
    }

    switch (type) {
        case INFO: strcat(msg, "INFO"); break;
        case MORE: strcat(msg, "MORE"); break;
        case ERRR: strcat(msg, "ERRR"); break;
    }

    strcat(msg, ": ");
    strcat(msg, format);
    strcat(msg, "\n");

    va_list args;
    va_start(args, format);

    vprintf(msg, args);

    va_end(args);
}
