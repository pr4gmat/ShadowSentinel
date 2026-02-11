#ifndef PROTECTION_H
#define PROTECTION_H

#include <sys/mman.h>

/*
   Структура для хранения атрибутов защиты страницы
*/
typedef struct {
    unsigned long raw;   // сырое значение (rwx в виде битов)
    int read;
    int write;
    int execute;
    int shared;
} page_protection_t;

/*
   Декодирование rwx из строки /proc/self/maps
*/
page_protection_t parse_protection(const char* perm);

/*
   Текстовое представление
*/
const char* protection_to_string(page_protection_t p);

#endif
