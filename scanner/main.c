#include "../common/log.h"
#include "../common/utils.h"
#include "../common/protection.h"

#include "va_scanner.h"
#include "module_list.h"
#include "memory_editor.h"

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

static void menu() {
    printf("\n=== Scanner menu ===\n");
    printf("1) Scan VA (/proc/self/maps)\n");
    printf("2) List loaded modules\n");
    printf("3) Find libstdc++ and show base\n");
    printf("4) Change protection of libstdc++ header page to RWX\n");
    printf("5) Zero libstdc++ header page (4096 bytes)\n");
    printf("6) dlopen/dlsym test (libm: cos)\n");
    printf("7) Map watcher file (watcher_data.bin)\n");
    printf("8) Corrupt mapped watcher file (write zeros to first page)\n");
    printf("0) Exit\n");
    printf("> ");
}

int main() {

    /* --- принудительная загрузка libstdc++ --- */
    void* h = dlopen("libstdc++.so.6", RTLD_LAZY);
    if (h) {
        log_msg(LOG_INFO, "libstdc++ forced load in scanner");
    } else {
        log_msg(LOG_WARNING, "dlopen libstdc++ failed: %s", dlerror());
    }

    module_list_t mods = {0};
    module_info_t lib = {0};

    void* w_addr = NULL;
    size_t w_size = 0;

    while (1) {
        int cmd = -1;
        menu();

        if (scanf("%d", &cmd) != 1)
            break;

        if (cmd == 0)
            break;

        if (cmd == 1) {

            va_map_t map = {0};
            if (va_scan_self(&map) == 0) {
                va_print(&map);
                va_free(&map);
            }

        } else if (cmd == 2) {

            modules_free(&mods);
            if (modules_from_maps(&mods) == 0) {
                modules_print(&mods);
            }

        } else if (cmd == 3) {

            if (!mods.modules)
                modules_from_maps(&mods);

            if (module_find_by_substr(&mods, "libstdc++", &lib) == 0) {
                log_msg(LOG_INFO,
                        "libstdc++ base=%p size=%zu name=%s",
                        lib.base, lib.size, lib.name);
            } else {
                log_msg(LOG_ERROR,
                        "libstdc++ not found in maps. (Try linking with libstdc++ or run watcher first)");
            }

        } else if (cmd == 4) {

            if (!mods.modules)
                modules_from_maps(&mods);

            if (module_find_by_substr(&mods, "libstdc++", &lib) != 0) {
                log_msg(LOG_ERROR, "libstdc++ not found");
                continue;
            }

            change_protection(lib.base,
                              get_page_size(),
                              PROT_READ | PROT_WRITE | PROT_EXEC);

        } else if (cmd == 5) {

            if (!mods.modules)
                modules_from_maps(&mods);

            if (module_find_by_substr(&mods, "libstdc++", &lib) != 0) {
                log_msg(LOG_ERROR, "libstdc++ not found");
                continue;
            }

            zero_memory(lib.base, get_page_size());

            log_msg(LOG_WARNING,
                    "libstdc++ header page corrupted (in-memory).");

        } else if (cmd == 6) {

            void* hm = dlopen("libm.so.6", RTLD_LAZY);
            if (!hm) {
                log_msg(LOG_ERROR,
                        "dlopen failed: %s",
                        dlerror());
                continue;
            }

            double (*cos_fn)(double) =
                (double (*)(double))dlsym(hm, "cos");

            char* err = dlerror();
            if (err) {
                log_msg(LOG_ERROR,
                        "dlsym failed: %s",
                        err);
                dlclose(hm);
                continue;
            }

            log_msg(LOG_INFO,
                    "cos(0.0) = %.2f (dlopen/dlsym OK)",
                    cos_fn(0.0));

            dlclose(hm);

        } else if (cmd == 7) {

            if (w_addr) {
                unmap_region(w_addr, w_size);
                w_addr = NULL;
                w_size = 0;
            }

            if (map_watcher_file("watcher_data.bin",
                                 &w_addr,
                                 &w_size) == 0) {
                log_msg(LOG_INFO,
                        "Mapped watcher_data.bin; addr=%p",
                        w_addr);
            }

        } else if (cmd == 8) {

            if (!w_addr) {
                log_msg(LOG_ERROR,
                        "Watcher file not mapped. Use option 7 first.");
                continue;
            }

            change_protection(w_addr,
                              get_page_size(),
                              PROT_READ | PROT_WRITE);

            zero_memory(w_addr, get_page_size());

            log_msg(LOG_WARNING,
                    "Mapped watcher file corrupted (first page zeroed).");

        } else {
            log_msg(LOG_WARNING, "Unknown command");
        }
    }

    if (w_addr)
        unmap_region(w_addr, w_size);

    modules_free(&mods);

    return 0;
}
