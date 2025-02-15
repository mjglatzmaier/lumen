#include "job_queue.h"
#include "lum_worker.h"
#include <stdatomic.h>
#include <stdlib.h>

// static WorkerThread workers[WORKER_COUNT];
// static atomic_int job_index = 0; // Round-robin style job submission

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

// void submit_job(Job *job) {
//     int index = atomic_fetch_add(&job_index, 1) % WORKER_COUNT;
//     push_job(&workers[index].queue, job);

//     // Notify all workers that work is available
//     lum_mutex_lock(&workers[index].queue_lock);
//     lum_cond_broadcast(&workers[index].job_available);
//     lum_mutex_unlock(&workers[index].queue_lock);
// }

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
        // Reset queue if empty
        atomic_store(&queue->top, t);
        atomic_store(&queue->bottom, t);
        return NULL;
    }
}

// Job* steal_job(WorkerThread *self) {
//     Job* job = NULL;
//     for (int k = 0; k < WORKER_COUNT; ++k) {
//         if (self != &workers[k] && (job = steal_job(&workers[k].queue) != NULL)) {
//             return job;
//         }
//     }
//     return NULL;
// }

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

// void execute_job(Job *job) {
//     job->function(job->data);
//     atomic_fetch_sub(&job->remaining_dependencies, 1);
// }

// void wait_for_job(Job *job) {
//     while (atomic_load(&job->remaining_dependencies) > 0) {
//         _mm_pause(); // Prevent busy-waiting
//     }
// }

// void job_system_init() {
//     for (int i = 0; i < WORKER_COUNT; i++) {
//         worker_thread_init(&workers[i]);
//     }
// }
