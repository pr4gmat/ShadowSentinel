#include "integrity.h"
#include "../common/log.h"
#include "../common/utils.h"

#include <unistd.h>
#include <dlfcn.h>

static int check_file(watcher_ctx_t* ctx) {
    unsigned long now =
        checksum(ctx->file.address, ctx->file.size);

    if (now != ctx->file_checksum) {
        log_msg(LOG_ERROR,
            "FILE MAPPING CHANGED! old=%lu new=%lu",
            ctx->file_checksum, now);
        return -1;
    }

    return 0;
}

/* Гарантированно подтягиваем libstdc++ в адресное пространство */
static void ensure_libstdc_loaded() {
    void* h = dlopen("libstdc++.so.6", RTLD_LAZY);
    if (h) {
        log_msg(LOG_INFO, "libstdc++ forced load via dlopen");
    } else {
        log_msg(LOG_WARNING, "dlopen(libstdc++.so.6) failed: %s", dlerror());
    }
}

int watcher_init(watcher_ctx_t* ctx) {
    const char* path = "watcher_data.bin";

    create_test_file(path, 100000);   // ~400 KB

    if (map_file(path, &ctx->file) < 0)
        return -1;

    ctx->file_checksum =
        checksum(ctx->file.address, ctx->file.size);

    /* --- ВАЖНО --- */
    ensure_libstdc_loaded();

    /* пытаемся найти библиотеку */
    if (find_std_library(&ctx->library) == 0) {
        /* нашли → читаем заголовок */
        read_library_header(&ctx->library, 4096);
    } else {
        /* не нашли → работаем только с файлом */
        log_msg(LOG_WARNING,
            "Library monitoring disabled (libstdc++ not present)");
        ctx->library.base = NULL;
    }

    ctx->interval_sec = 2;

    return 0;
}

void watcher_loop(watcher_ctx_t* ctx) {
    log_msg(LOG_INFO, "Watcher started");

    while (1) {
        check_file(ctx);

        /* проверяем библиотеку только если она есть */
        if (ctx->library.base) {
            verify_library(&ctx->library);
        }

        sleep(ctx->interval_sec);
    }
}
