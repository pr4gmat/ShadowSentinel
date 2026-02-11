#include "va_scanner.h"
#include "../common/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_maps_line(const char* line, va_region_t* r) {
    unsigned long start, end, offset, inode;
    char perms[8] = {0};
    char dev[16] = {0};

    int n = sscanf(line, "%lx-%lx %7s %lx %15s %lu",
                   &start, &end, perms, &offset, dev, &inode);
    if (n < 6) return -1;

    memset(r, 0, sizeof(*r));

    r->start = (void*)start;
    r->end   = (void*)end;
    r->prot  = parse_protection(perms);
    r->offset = offset;

    /* безопасное копирование dev */
    snprintf(r->dev, sizeof(r->dev), "%s", dev);
    r->inode = inode;

    const char* p = strchr(line, '/');
    if (p) {
        r->has_path = 1;

        /* безопасное копирование пути */
        snprintf(r->path, sizeof(r->path), "%s", p);

        /* убрать перевод строки */
        size_t len = strlen(r->path);
        if (len && r->path[len-1] == '\n')
            r->path[len-1] = '\0';
    } else {
        r->has_path = 0;
        r->path[0] = '\0';
    }

    return 0;
}

int va_scan_self(va_map_t* out) {
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) {
        log_msg(LOG_ERROR, "Cannot open /proc/self/maps");
        return -1;
    }

    size_t cap = 128;
    out->regions = (va_region_t*)calloc(cap, sizeof(va_region_t));
    out->count = 0;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        if (out->count == cap) {
            cap *= 2;
            out->regions = (va_region_t*)realloc(
                out->regions, cap * sizeof(va_region_t));
        }

        if (parse_maps_line(line, &out->regions[out->count]) == 0) {
            out->count++;
        }
    }

    fclose(f);
    return 0;
}

void va_free(va_map_t* map) {
    if (!map) return;
    free(map->regions);
    map->regions = NULL;
    map->count = 0;
}

void va_print(const va_map_t* map) {
    for (size_t i = 0; i < map->count; i++) {
        const va_region_t* r = &map->regions[i];

        printf("%p-%p  %s  off=%lx  dev=%s inode=%lu",
               r->start, r->end,
               protection_to_string(r->prot),
               r->offset, r->dev, r->inode);

        if (r->has_path)
            printf("  %s", r->path);

        printf("\n");
    }
}
