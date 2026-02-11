#ifndef LOG_H
#define LOG_H

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG
} log_level_t;

void log_msg(log_level_t lvl, const char* fmt, ...);

#endif
