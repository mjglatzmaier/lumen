#include "job_scheduler.h"
#include "lum_worker.h"
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>

static WorkerThread workers[WORKER_COUNT];
static atomic_int job_index = 0;

void job_scheduler_init() {
    for (int i = 0; i < WORKER_COUNT; i++) {
        lum_thread_init(&workers[i], i);
    }
}

void job_scheduler_submit(Job *job) {
    // Select worker using round-robin
    int index = atomic_fetch_add(&job_index, 1) % WORKER_COUNT;

    push_job(&workers[index].queue, job);

    // Notify all workers (config dependent?)
    lum_mutex_lock(&workers[index].queue_lock);
    lum_cond_signal(&workers[index].job_available);
    lum_mutex_unlock(&workers[index].queue_lock);
}

void job_scheduler_shutdown() {
    for (int i = 0; i < WORKER_COUNT; i++) {
        lum_thread_shutdown(&workers[i]);
    }
}
