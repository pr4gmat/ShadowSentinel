#ifndef VA_SCANNER_H
#define VA_SCANNER_H

#include <stddef.h>
#include "../common/protection.h"

typedef struct {
    void* start;
    void* end;
    page_protection_t prot;
    unsigned long offset;
    char dev[16];
    unsigned long inode;
    char path[256];     // может быть пустым

    int has_path;
} va_region_t;

typedef struct {
    va_region_t* regions;
    size_t count;
} va_map_t;

int  va_scan_self(va_map_t* out);
void va_free(va_map_t* map);
void va_print(const va_map_t* map);

#endif
