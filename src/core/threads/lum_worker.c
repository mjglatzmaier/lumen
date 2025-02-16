#include "lum_worker.h"
#include "job_queue.h"
#include "../memory/allocators/mem_pool.h"
#include <stdlib.h>
#include <stdio.h>


// Helper to find a job
Job * find_work(WorkerThread* self) {
    Job *job = pop_job(&self->queue);
    if (job != NULL)
       return job;
    // else if ((job = steal_job(&self->queue)) != NULL) {
    //     printf("Worker %d: No jobs, attempting to steal...\n", self->id);
    // }
    return NULL;
}

void execute_job(WorkerThread* worker, Job *job) {
    job->function(job->data);
    atomic_fetch_sub(&job->remaining_dependencies, 1);
    worker->allocator->free(worker->allocator, job);
}

void *worker_thread_function(void *arg) {
    WorkerThread *self = (WorkerThread *)arg;

    while (atomic_load(&self->running)) {
        Job *job = find_work(self);

        if (job != NULL) {
            execute_job(self, job);
            printf("Worker %d executing job...\n", self->id);
        }
        else {
            printf("Worker %d going to sleep.\n", self->id);
            // No work available, wait for a job
            lum_mutex_lock(&self->queue_lock);
            while ((job = pop_job(&self->queue)) == NULL && atomic_load(&self->running)) {
                printf("Worker %d is now waiting.\n", self->id);
                lum_cond_wait(&self->job_available, &self->queue_lock);
                printf("Worker %d woke up! Checking for jobs...\n", self->id);
            }
            lum_mutex_unlock(&self->queue_lock);

            // Try stealing again after being woken up
            // if (job == NULL) {
            //     job = steal_job(&self->queue);
            // }

            if (job != NULL) {
                printf("Worker %d woke up and executing job...\n", self->id);
                execute_job(self, job);
            }
            printf("Worker %d woke up but no job found...\n", self->id);
        }
    }
    return NULL;
}

void lum_thread_init(WorkerThread *worker, int id) {
    lum_mutex_init(&worker->queue_lock);
    lum_cond_init(&worker->job_available);
    worker->id = id;
    printf("Initializing Worker %d\n", id); // Confirm worker IDs
    atomic_store(&worker->running, true);
    worker->allocator = lum_create_pool_allocator(sizeof(Job), JOB_QUEUE_SIZE);
    worker->thread = lum_create_thread(worker_thread_function, worker);
}

// Stop worker thread (graceful shutdown)
void lum_thread_shutdown(WorkerThread *worker) {
    atomic_store(&worker->running, false);

    // Wake up any sleeping worker
    lum_mutex_lock(&worker->queue_lock);
    lum_cond_broadcast(&worker->job_available);
    lum_mutex_unlock(&worker->queue_lock);

    lum_thread_join(worker->thread);
    lum_mutex_destroy(&worker->queue_lock);
    lum_cond_destroy(&worker->job_available);
    lum_pool_allocator_destroy(worker->allocator);
}
