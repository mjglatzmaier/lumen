#ifndef LUM_SCHEDULER_H
#define LUM_SCHEDULER_H

#include "containers/cont_lfq.h"
#include "threads/lum_thread.h"

typedef struct lum_allocator lum_allocator;

typedef enum
{
    LUM_SCHEDULER_ROUND_ROBIN,
    LUM_SCHEDULER_WORK_STEALING
} lum_scheduler_type;

typedef enum
{
    LUM_WAIT_COND_VAR, // Default: Use condition variables (Best CPU usage)
    LUM_WAIT_BACKOFF,  // Use sched_yield + incremental sleep (Low latency)
    LUM_WAIT_SPINLOCK  // Spin-wait (High-performance networking, rendering)
} lum_wait_policy_t;

typedef struct
{
    lum_wait_policy_t  wait_policy;
    lum_scheduler_type type;
    lum_thread_type    thread_type; // Task or NUMA
    size_t             num_threads;
    size_t             queue_capacity;
    lum_allocator     *allocator;
    lum_lfq_t         *queue;
    lum_thread_t      *threads;
} lum_scheduler_config_t;

typedef struct
{
    lum_scheduler_config_t *config;
    lum_mutex               submission_lock;
    lum_mutex               job_lock;
    lum_cond_var            job_available;
    lum_cond_var            job_done;
    atomic_int              jobs_remaining;
    atomic_bool             running;
} lum_scheduler_t;

// typedef struct {
//     lum_scheduler_t* schedulers[MAX_SCHEDULERS];
//     size_t count;
// } lum_scheduler_manager_t;

// API
lum_scheduler_t *lum_scheduler_create(lum_scheduler_config_t *config);
Job *lum_scheduler_create_job(lum_scheduler_t *scheduler, lum_thread_func function, void *data);
void lum_scheduler_submit(lum_scheduler_t *scheduler, Job *job);
void lum_scheduler_submit_batch(lum_scheduler_t *scheduler, Job **jobs, size_t count);
void lum_scheduler_wait_completion(lum_scheduler_t *scheduler);
void lum_scheduler_destroy(lum_scheduler_t *scheduler);

#endif // LUM_SCHEDULER_H