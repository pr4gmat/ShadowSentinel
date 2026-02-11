#include "library_monitor.h"
#include "../common/log.h"
#include "../common/utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
   Ищем строку с libstdc в /proc/self/maps
*/
int find_std_library(library_watch_t* lib) {
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) {
        log_msg(LOG_ERROR, "Cannot open /proc/self/maps");
        return -1;
    }

    char line[512];

    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "libstdc++")) {

            unsigned long start, end;
            sscanf(line, "%lx-%lx", &start, &end);

            lib->base = (void*)start;

            char* path = strchr(line, '/');
            if (path) {
                strncpy(lib->path, path, sizeof(lib->path)-1);
                lib->path[strcspn(lib->path, "\n")] = 0;
            }

            fclose(f);

            log_msg(LOG_INFO,
                "Found libstdc++ at %p (%s)",
                lib->base, lib->path);

            return 0;
        }
    }

    fclose(f);
    log_msg(LOG_ERROR, "libstdc++ not found");
    return -1;
}

int read_library_header(library_watch_t* lib, size_t size) {
    lib->header_size = size;

    lib->checksum = checksum(lib->base, size);

    log_msg(LOG_INFO,
        "Library header checksum = %lu",
        lib->checksum);

    return 0;
}

int verify_library(library_watch_t* lib) {
    unsigned long now = checksum(lib->base, lib->header_size);

    if (now != lib->checksum) {
        log_msg(LOG_ERROR,
            "LIBRARY CHANGED! old=%lu new=%lu",
            lib->checksum, now);
        return -1;
    }

    return 0;
}
