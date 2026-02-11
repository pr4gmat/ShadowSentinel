#ifndef LIBRARY_MONITOR_H
#define LIBRARY_MONITOR_H

#include <stddef.h>

typedef struct {
    void*  base;
    size_t header_size;
    unsigned long checksum;
    char   path[256];
} library_watch_t;

/*
   Поиск libstdc++.so в /proc/self/maps
*/
int find_std_library(library_watch_t* lib);

/*
   Чтение первых N байт как "заголовка"
*/
int read_library_header(library_watch_t* lib, size_t size);

/*
   Проверка неизменности
*/
int verify_library(library_watch_t* lib);

#endif
