#include "job_scheduler.h"
#include "lum_worker.h"
#include "../memory/allocators/mem_alloc.h"
#include "platform.h"
#include <stdatomic.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define SCHEDULER_ALIGNMENT 16

// Round-Robin Scheduler Implementation
static JobScheduler round_robin_scheduler = {
    .workers = NULL,
    .worker_count = 0,
    .allocator = NULL,
    .init = job_scheduler_round_robin_init,
    .submit_job = job_scheduler_round_robin_submit,
    .shutdown = job_scheduler_round_robin_shutdown
};

// TODO: should the scheduler be a singleton? Or should we allow multiple instances?
JobScheduler * job_scheduler_init(int requested_worker_count, SchedulerType type) {
    assert(requested_worker_count > 0 && "Worker count must be positive!");
    JobScheduler *scheduler = NULL;
    switch (type) {
        case SCHEDULER_ROUND_ROBIN:
            // TODO: make a fresh one?
            scheduler = create_round_robin_scheduler(requested_worker_count);
            break;
        // case SCHEDULER_WORK_STEALING:
        //     scheduler = &work_stealing_scheduler;
        //     break;
        default:
            printf("Error: Invalid scheduler type!\n");
            return;
    }
}


void job_scheduler_shutdown(JobScheduler* scheduler) {
    assert(scheduler != NULL && "Scheduler not initialized!");
    scheduler->shutdown(scheduler);
    scheduler = NULL;
}

// Default scheduler implementations
static void job_scheduler_default_init(JobScheduler* self, int worker_count) {
    assert(self->worker_count > 0 && "Worker count must be positive!");

    for (int i = 0; i < worker_count; i++) {
        lum_thread_init(&self->workers[i], i);
    }
}


// Round-Robin Scheduler Implementation
static JobScheduler * create_round_robin_scheduler(int worker_count) {
    // TODO: allow custom allocator?
    lum_allocator * allocator = lum_create_default_allocator();
    assert(allocator != NULL && "Failed to create default allocator!");

    JobScheduler *scheduler = allocator->alloc(allocator, sizeof(JobScheduler), SCHEDULER_ALIGNMENT);
    assert(scheduler != NULL && "Failed to allocate job scheduler!");
    scheduler->allocator = allocator;
    scheduler->worker_count = worker_count;

    scheduler->workers = allocator->alloc(allocator, worker_count * sizeof(WorkerThread), SCHEDULER_ALIGNMENT);
    assert(scheduler->workers != NULL && "Failed to allocate worker threads!");
    atomic_store_explicit(&scheduler->job_index, 0, memory_order_release);

    scheduler->init         = job_scheduler_default_init;
    scheduler->submit_job   = job_scheduler_round_robin_submit;
    scheduler->shutdown     = job_scheduler_round_robin_shutdown;
}


static void job_scheduler_round_robin_submit(JobScheduler* self, JobFunction func, void *data) {

    // Select worker using round-robin
    int index = atomic_fetch_add(&self->job_index, 1) % WORKER_COUNT;  
    WorkerThread *worker = &self->workers[index];
    assert(worker != NULL && "Worker not initialized!");

    push_job(worker, func, data);

    // Ensure the job is fully visible before waking workers
    atomic_thread_fence(memory_order_release);
    
    printf("Signaling Worker %d after job submission\n", index);
    lum_mutex_lock(&worker->queue_lock);
    lum_cond_broadcast(&worker->job_available);
    lum_mutex_unlock(&worker->queue_lock);
}


static void job_scheduler_shutdown() {
    for (int i = 0; i < WORKER_COUNT; i++) {
        lum_thread_shutdown(&workers[i]);
    }
}
