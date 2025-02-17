#include "lum_thread.h"
#include "../memory/allocators/mem_pool.h"
#include <stdlib.h>
#include <stdio.h>

void lum_thread_init(lum_thread_t *worker, int id, lum_thread_func func, void* arg) {
    printf("Initializing Worker %d\n", id);
    atomic_store(&worker->running, true);
    worker->thread = lum_thread_create(func, arg);
}

// Stop worker thread (graceful shutdown)
void lum_thread_shutdown(lum_thread_t *worker) {
    atomic_store(&worker->running, false);
    lum_thread_join(worker->thread);
}
