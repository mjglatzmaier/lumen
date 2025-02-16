#ifndef LUMEN_WORKER_THREAD_H
#define LUMEN_WORKER_THREAD_H

#include <stdbool.h>
#include <stdatomic.h>
#include "../platform.h"
#include "job_queue.h"

typedef struct lum_allocator lum_allocator;

typedef struct WorkerThread {
    lum_thread_id id;
    JobQueue queue;
    lum_thread thread;
    lum_mutex queue_lock;
    lum_cond_var job_available;
    lum_allocator* allocator;
    atomic_bool running;
} WorkerThread;

void lum_thread_init(WorkerThread *worker, int id);
void lum_thread_shutdown(WorkerThread *worker);

#endif
