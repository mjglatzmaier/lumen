#include "job_queue.h"
#include "lum_worker.h"
#include <stdatomic.h>
#include <stdlib.h>

void push_job(JobQueue *queue, Job *job) {
    int b = atomic_load(&queue->bottom);
    int t = atomic_load(&queue->top);

    if ((b - t) >= JOB_QUEUE_SIZE) {
        //printf("Warning: Job queue overflow! Dropping job.\n");
        return;
    }

    queue->jobs[b % JOB_QUEUE_SIZE] = job;
    atomic_thread_fence(memory_order_seq_cst);
    atomic_store(&queue->bottom, b + 1);
}

Job *pop_job(JobQueue *queue) {
    int b = atomic_load(&queue->bottom) - 1;
    atomic_store(&queue->bottom, b);
    atomic_thread_fence(memory_order_seq_cst);
    int t = atomic_load(&queue->top);

    if (b >= t) {
        Job *job = queue->jobs[b % JOB_QUEUE_SIZE];
        return job;
    } else {
        // Reset queue if empty
        atomic_store(&queue->top, t);
        atomic_store(&queue->bottom, t);
        return NULL;
    }
}

Job *steal_job(JobQueue *queue) {
    int t = atomic_load(&queue->top);
    atomic_thread_fence(memory_order_seq_cst);
    int b = atomic_load(&queue->bottom);

    if (t < b) {
        Job *job = queue->jobs[t % JOB_QUEUE_SIZE];
        atomic_store(&queue->top, t + 1);
        return job;
    } else {
        return NULL;
    }
}