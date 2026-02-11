#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static const char* level_str(log_level_t lvl) {
    switch (lvl) {
        case LOG_INFO:    return "INFO";
        case LOG_WARNING: return "WARN";
        case LOG_ERROR:   return "ERROR";
        case LOG_DEBUG:   return "DEBUG";
        default:          return "UNK";
    }
}

void log_msg(log_level_t lvl, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    time_t t = time(NULL);
    struct tm* tm = localtime(&t);

    printf("[%02d:%02d:%02d] [%s] ",
           tm->tm_hour, tm->tm_min, tm->tm_sec,
           level_str(lvl));

    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}
