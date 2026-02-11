#include "memory_editor.h"
#include "../common/log.h"
#include "../common/utils.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int change_protection(void* addr, size_t size, int prot) {
    void* page = align_page(addr);
    size_t page_size = get_page_size();

    // на всякий: делаем минимум одну страницу
    size_t len = (size < page_size) ? page_size : size;

    if (mprotect(page, len, prot) != 0) {
        log_msg(LOG_ERROR, "mprotect failed: %s", strerror(errno));
        return -1;
    }

    log_msg(LOG_INFO, "mprotect(%p, %zu, prot=%d) OK", page, len, prot);
    return 0;
}

int zero_memory(void* addr, size_t size) {
    memset(addr, 0, size);
    log_msg(LOG_INFO, "Memory at %p zeroed (%zu bytes)", addr, size);
    return 0;
}

int map_watcher_file(const char* path, void** out_addr, size_t* out_size) {
    int fd = open(path, O_RDWR);
    if (fd < 0) {
        log_msg(LOG_ERROR, "Cannot open watcher file: %s", path);
        return -1;
    }

    size_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (addr == MAP_FAILED) {
        log_msg(LOG_ERROR, "mmap watcher file failed");
        return -1;
    }

    *out_addr = addr;
    *out_size = size;

    log_msg(LOG_INFO, "Watcher file mapped at %p, size=%zu", addr, size);
    return 0;
}

void unmap_region(void* addr, size_t size) {
    if (!addr || !size) return;
    munmap(addr, size);
}
