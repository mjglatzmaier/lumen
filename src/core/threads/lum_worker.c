#include "lum_worker.h"
#include "job_queue.h"
#include <stdlib.h>
#include <stdio.h>

static atomic_int steal_attempts;

// Helper to find a job
Job * find_work(WorkerThread* self) {
    Job *job = pop_job(&self->queue);
    if (job != NULL)
       return job;
    else if ((job = steal_job(&self->queue)) != NULL) {
        atomic_fetch_add(&steal_attempts, 1);
    }
    return NULL;
}

inline void execute_job(Job *job) {
    job->function(job->data);
    atomic_fetch_sub(&job->remaining_dependencies, 1);
}

void *worker_thread_function(void *arg) {
    WorkerThread *self = (WorkerThread *)arg;

    while (atomic_load(&self->running)) {
        Job *job = find_work(self);

        if (job != NULL)
            execute_job(job);
        else {
            // No work available, wait for a job
            lum_mutex_lock(&self->queue_lock);
            while ((job = pop_job(&self->queue)) == NULL && atomic_load(&self->running)) {
                lum_cond_wait(&self->job_available, &self->queue_lock);
            }
            lum_mutex_unlock(&self->queue_lock);

            // Try stealing again after being woken up
            if (job == NULL)
            {
                job = steal_job(&self->queue);
                if (job != NULL) {
                    atomic_fetch_add(&steal_attempts, 1);
                }
            }

            if (job != NULL)
                execute_job(job);
        }
    }
    return NULL;
}

void lum_thread_init(WorkerThread *worker, int id) {
    lum_mutex_init(&worker->queue_lock);
    lum_cond_init(&worker->job_available);
    worker->id = id;
    atomic_store(&worker->running, true);
    worker->thread = lum_create_thread(worker_thread_function, worker);
}

// Stop worker thread (graceful shutdown)
void lum_thread_shutdown(WorkerThread *worker) {
    printf("Stole work %d times\n", steal_attempts);
    atomic_store(&worker->running, false);

    // Wake up any sleeping worker
    lum_mutex_lock(&worker->queue_lock);
    lum_cond_broadcast(&worker->job_available);
    lum_mutex_unlock(&worker->queue_lock);

    lum_thread_join(worker->thread);
    lum_mutex_destroy(&worker->queue_lock);
    lum_cond_destroy(&worker->job_available);
}
