#ifndef FILE_MAPPER_H
#define FILE_MAPPER_H

#include <stddef.h>

typedef struct {
    void*  address;
    size_t size;
    int    fd;
} mapped_file_t;

/*
   Создание тестового файла с числами 0..count-1
*/
int create_test_file(const char* path, size_t count);

/*
   Проецирование файла в память (mmap)
*/
int map_file(const char* path, mapped_file_t* out);

/*
   Размонтирование
*/
void unmap_file(mapped_file_t* mf);

#endif
