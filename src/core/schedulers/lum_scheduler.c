#include "lum_scheduler.h"

#include "../memory/allocators/mem_alloc.h"
#include "lum_thread.h"
#include "platform.h"

#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define SCHEDULER_ALIGNMENT 16

Job *lum_scheduler_create_job(lum_scheduler_t *scheduler, lum_thread_func function, void *data)
{
    if (!scheduler || !function)
        return NULL;
    lum_allocator *allocator = scheduler->config->allocator;

    Job *job = allocator->alloc(allocator, sizeof(Job), 16);
    if (!job)
        return NULL;

    job->function = function;
    job->data     = data;
    return job;
}

void execute_job(Job *job, lum_scheduler_t *s)
{
    if (!job)
        return;

    // Ensure job function is valid
    assert(job->function != NULL && "Job function is NULL!");

    // Execute the job function with the provided data
    job->function(job->data);
    lum_mutex_lock(&s->job_lock);
    if (atomic_fetch_sub(&s->jobs_remaining, 1) == 1)
    {
        lum_cond_signal(&s->job_done); // Signal main thread if last job
    }
    lum_mutex_unlock(&s->job_lock);
    if (s)
    {
        s->config->allocator->free(s->config->allocator, job);
    }
}

void *worker_thread_function(void *arg)
{
    lum_scheduler_t *s = (lum_scheduler_t *) arg;
    if (!s)
        return NULL;

    while (atomic_load(&s->running))
    {
        Job *job = lum_lfq_dequeue(s->config->queue);

        // Fast spin before sleeping
        // int spin_count = 1000;
        // while (!job && spin_count--) {
        //     sched_yield();
        //     job = lum_lfq_dequeue(s->config->queue);
        // }

        if (!job)
        {
            // Queue is empty, wait for a new job
            lum_mutex_lock(&s->job_lock);
            // Wait until we are explicitly signaled that work is available
            while (atomic_load(&s->running) && lum_lfq_empty(s->config->queue)) {
                lum_cond_wait(&s->job_available, &s->job_lock);
            }
            
            job = lum_lfq_dequeue(s->config->queue);
            lum_mutex_unlock(&s->job_lock);
        }
        
        if (job)
            execute_job(job, s);
    }
    return NULL;
}

lum_scheduler_t *lum_scheduler_create(lum_scheduler_config_t *config)
{
    // Allocator
    if (!config->allocator)
        config->allocator = lum_create_default_allocator();
    if (!config->allocator)
        return NULL;
    // Make a local copy for convenience
    lum_allocator *allocator = config->allocator;

    // Queue
    if (!config->queue)
    {
        if (config->queue_capacity <= 0)
            config->queue_capacity = 256;
        config->queue = allocator->alloc(allocator, sizeof(lum_lfq_t), SCHEDULER_ALIGNMENT);
        if (!config->queue)
        {
            allocator->free(allocator, allocator);
            return NULL;
        }
    }

    // Scheduler
    lum_scheduler_t *scheduler = allocator->alloc(allocator, sizeof(lum_scheduler_t), 16);
    if (!scheduler)
    {
        allocator->free(allocator, allocator);
        return NULL;
    }

    scheduler->config  = config;
    scheduler->running = true;

    // TODO: should the queue have its own allocator?
    lum_lfq_init(config->queue, config->queue_capacity, allocator);

    // Threads
    if (config->num_threads <= 0)
        config->num_threads = 4;
    if (config->thread_type == 0) // Default if unset
        config->thread_type = LUM_THREAD_TASK;
    if (!config->threads)
    {
        config->threads = allocator->alloc(allocator, config->num_threads * sizeof(lum_thread_t),
                                           SCHEDULER_ALIGNMENT);
        if (!config->threads)
        {
            lum_scheduler_destroy(scheduler);
            return NULL;
        }
    }

    atomic_store(&scheduler->jobs_remaining, 0);
    lum_mutex_init(&scheduler->submission_lock);
    lum_mutex_init(&scheduler->job_lock);
    lum_cond_init(&scheduler->job_available);
    lum_cond_init(&scheduler->job_done);

    // Launch threads
    for (size_t i = 0; i < config->num_threads; i++)
    {
        lum_thread_init(&config->threads[i], i, worker_thread_function, (void *) scheduler);
    }

    return scheduler;
}

void lum_scheduler_submit_batch(lum_scheduler_t *scheduler, Job **jobs, size_t count)
{
    lum_mutex_lock(&scheduler->submission_lock);
    bool did_submit = false;
    for (size_t i = 0; i < count; i++)
    {
        if (!lum_lfq_enqueue(scheduler->config->queue, jobs[i]))
        {
            printf("Queue is full! Job submission failed.\n");
            break;
        }
        else
            did_submit = true;
    }
    if (did_submit)
        lum_cond_signal(&scheduler->job_available);
    lum_mutex_unlock(&scheduler->submission_lock);
}

void lum_scheduler_submit(lum_scheduler_t *scheduler, Job *job)
{
    // TODO: this queue should be lockless - refactor.
    //lum_mutex_lock(&scheduler->submission_lock);
    if (!lum_lfq_enqueue(scheduler->config->queue, job))
    {
        printf("Queue is full! Job submission failed. Capacity %zu.\n",
            scheduler->config->queue->capacity);
    }
    else
    {
        atomic_fetch_add(&scheduler->jobs_remaining, 1);
        lum_cond_signal(&scheduler->job_available);
    }
    //lum_mutex_unlock(&scheduler->submission_lock);
}

void lum_scheduler_wait_completion(lum_scheduler_t *scheduler)
{
    lum_mutex_lock(&scheduler->job_lock);
    while (atomic_load(&scheduler->jobs_remaining) > 0)
    {
        lum_cond_wait(&scheduler->job_done, &scheduler->job_lock);
    }
    lum_mutex_unlock(&scheduler->job_lock);
}

void lum_scheduler_destroy(lum_scheduler_t *scheduler)
{
    if (!scheduler)
        return;

    // TODO: investigate efficiency of this barrier
    atomic_thread_fence(memory_order_seq_cst);

    // Signal threads to stop
    atomic_store(&scheduler->running, false);
    lum_cond_broadcast(&scheduler->job_available); // Flush.

    // Join all worker threads
    for (size_t i = 0; i < scheduler->config->num_threads; i++)
    {
        lum_thread_join(scheduler->config->threads[i].thread);
    }

    lum_mutex_destroy(&scheduler->submission_lock);
    lum_mutex_destroy(&scheduler->job_lock);
    lum_cond_destroy(&scheduler->job_available);
    lum_cond_destroy(&scheduler->job_done);
    if (scheduler->config->queue)
    {
        scheduler->config->allocator->free(scheduler->config->allocator, scheduler->config->queue);
        scheduler->config->queue = NULL;
    }

    if (scheduler->config->threads)
    {
        scheduler->config->allocator->free(scheduler->config->allocator, scheduler->config->threads);
        scheduler->config->threads = NULL;
    }

    if (scheduler)
    {
        scheduler->config->allocator->free(scheduler->config->allocator, scheduler);
        scheduler = NULL;
    }
    // Note: the allocator is passed in by the client, so it is not freed.
}