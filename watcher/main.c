#include "integrity.h"
#include "../common/log.h"

int main() {
    watcher_ctx_t ctx;

    if (watcher_init(&ctx) < 0) {
        log_msg(LOG_ERROR, "Init failed");
        return 1;
    }

    watcher_loop(&ctx);

    return 0;
}
