#ifndef MEMORY_EDITOR_H
#define MEMORY_EDITOR_H

#include <stddef.h>

/*
   Смена прав страницы (mprotect)
*/
int change_protection(void* addr, size_t size, int prot);

/*
   Зануление участка памяти
*/
int zero_memory(void* addr, size_t size);

/*
   mmap файла сторожа в сканер
*/
int map_watcher_file(const char* path, void** out_addr, size_t* out_size);

/*
   размонтирование mmap
*/
void unmap_region(void* addr, size_t size);

#endif
