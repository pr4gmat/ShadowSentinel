#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>

typedef enum {
    OS_LINUX = 1
} os_type_t;

static inline os_type_t get_os_type() {
    return OS_LINUX;
}

#endif
