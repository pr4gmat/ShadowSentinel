#include "module_list.h"
#include "../common/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char path[256];
    unsigned long min_start;
    unsigned long max_end;
} agg_t;

static int agg_find(agg_t* a, size_t n, const char* path) {
    for (size_t i = 0; i < n; i++) {
        if (strcmp(a[i].path, path) == 0)
            return (int)i;
    }
    return -1;
}

int modules_from_maps(module_list_t* out) {
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) {
        log_msg(LOG_ERROR, "Cannot open /proc/self/maps");
        return -1;
    }

    size_t cap = 64;
    agg_t* aggs = (agg_t*)calloc(cap, sizeof(agg_t));
    size_t agg_count = 0;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        unsigned long start, end;
        char perms[8], dev[16];
        unsigned long offset, inode;

        int n = sscanf(line, "%lx-%lx %7s %lx %15s %lu",
                       &start, &end, perms, &offset, dev, &inode);
        if (n < 6) continue;

        char* path = strchr(line, '/');
        if (!path) continue;

        char clean[256];
        snprintf(clean, sizeof(clean), "%s", path);

        /* убрать перевод строки */
        size_t len = strlen(clean);
        if (len && clean[len-1] == '\n')
            clean[len-1] = '\0';

        int idx = agg_find(aggs, agg_count, clean);

        if (idx < 0) {
            if (agg_count == cap) {
                cap *= 2;
                aggs = (agg_t*)realloc(aggs, cap * sizeof(agg_t));
            }

            snprintf(aggs[agg_count].path,
                     sizeof(aggs[agg_count].path),
                     "%s", clean);

            aggs[agg_count].min_start = start;
            aggs[agg_count].max_end   = end;
            agg_count++;
        } else {
            if (start < aggs[idx].min_start)
                aggs[idx].min_start = start;

            if (end > aggs[idx].max_end)
                aggs[idx].max_end = end;
        }
    }

    fclose(f);

    out->modules = (module_info_t*)calloc(
        agg_count, sizeof(module_info_t));
    out->count = agg_count;

    for (size_t i = 0; i < agg_count; i++) {
        snprintf(out->modules[i].name,
                 sizeof(out->modules[i].name),
                 "%s", aggs[i].path);

        out->modules[i].base = (void*)aggs[i].min_start;
        out->modules[i].size =
            (size_t)(aggs[i].max_end - aggs[i].min_start);
    }

    free(aggs);
    return 0;
}

void modules_free(module_list_t* list) {
    if (!list) return;
    free(list->modules);
    list->modules = NULL;
    list->count = 0;
}

void modules_print(const module_list_t* list) {
    for (size_t i = 0; i < list->count; i++) {
        printf("%p  size=%zu  %s\n",
               list->modules[i].base,
               list->modules[i].size,
               list->modules[i].name);
    }
}

int module_find_by_substr(const module_list_t* list,
                          const char* needle,
                          module_info_t* out) {
    for (size_t i = 0; i < list->count; i++) {
        if (strstr(list->modules[i].name, needle)) {
            if (out)
                *out = list->modules[i];
            return 0;
        }
    }
    return -1;
}
