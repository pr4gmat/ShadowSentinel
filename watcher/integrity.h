#ifndef INTEGRITY_H
#define INTEGRITY_H

#include "file_mapper.h"
#include "library_monitor.h"

typedef struct {
    mapped_file_t   file;
    library_watch_t library;

    unsigned long file_checksum;

    int interval_sec;
} watcher_ctx_t;

int  watcher_init(watcher_ctx_t* ctx);
void watcher_loop(watcher_ctx_t* ctx);

#endif
