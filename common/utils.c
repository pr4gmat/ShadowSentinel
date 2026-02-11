#include "utils.h"
#include <unistd.h>
#include <stdint.h>

unsigned long checksum(const void* data, size_t size) {
    const unsigned char* p = (const unsigned char*)data;
    unsigned long sum = 0;

    for (size_t i = 0; i < size; i++) {
        sum = (sum << 5) + sum + p[i];   // djb2-like
    }

    return sum;
}

void* align_page(void* addr) {
    size_t page = get_page_size();
    return (void*)((uintptr_t)addr & ~(page - 1));
}

size_t get_page_size() {
    return (size_t)sysconf(_SC_PAGESIZE);
}
