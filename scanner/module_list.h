#ifndef MODULE_LIST_H
#define MODULE_LIST_H

#include <stddef.h>

typedef struct {
    char name[256];
    void* base;
    size_t size;
} module_info_t;

typedef struct {
    module_info_t* modules;
    size_t count;
} module_list_t;

int  modules_from_maps(module_list_t* out);
void modules_free(module_list_t* list);
void modules_print(const module_list_t* list);

int  module_find_by_substr(const module_list_t* list, const char* needle, module_info_t* out);

#endif
