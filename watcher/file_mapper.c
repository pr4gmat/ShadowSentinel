#include "file_mapper.h"
#include "../common/log.h"
#include "../common/utils.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>

/*
   Создаем файл с последовательностью uint32_t: 0,1,2...
*/
int create_test_file(const char* path, size_t count) {
    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        log_msg(LOG_ERROR, "Cannot create file %s", path);
        return -1;
    }

    for (uint32_t i = 0; i < count; i++) {
        write(fd, &i, sizeof(i));
    }

    close(fd);

    log_msg(LOG_INFO, "Test file created: %s (%zu numbers)", path, count);
    return 0;
}

int map_file(const char* path, mapped_file_t* out) {
    int fd = open(path, O_RDWR);
    if (fd < 0) {
        log_msg(LOG_ERROR, "Cannot open file %s", path);
        return -1;
    }

    size_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    void* addr = mmap(NULL, size,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      fd, 0);

    if (addr == MAP_FAILED) {
        log_msg(LOG_ERROR, "mmap failed");
        close(fd);
        return -1;
    }

    out->address = addr;
    out->size = size;
    out->fd = fd;

    log_msg(LOG_INFO, "File mapped at %p, size=%zu", addr, size);

    return 0;
}

void unmap_file(mapped_file_t* mf) {
    if (!mf || !mf->address)
        return;

    munmap(mf->address, mf->size);
    close(mf->fd);

    mf->address = NULL;
}
