#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/*
   Контрольная сумма области памяти
*/
unsigned long checksum(const void* data, size_t size);

/*
   Выравнивание адреса на границу страницы
*/
void* align_page(void* addr);

/*
   Получение размера страницы
*/
size_t get_page_size();

#endif
