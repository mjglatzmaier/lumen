#ifndef JOB_SCHEDULER_H
#define JOB_SCHEDULER_H

#include "job_queue.h"

typedef struct WorkerThread WorkerThread;
typedef struct lum_allocator lum_allocator;

typedef struct JobScheduler {
    WorkerThread* workers;
    int worker_count;
    atomic_int job_index;
    lum_allocator* allocator;
    void (*init)(JobScheduler* self, int worker_count); 
    void (*submit_job)(JobScheduler* self, JobFunction func, void *data);
    void (*shutdown)(JobScheduler* self);
} JobScheduler;

typedef enum {
    SCHEDULER_ROUND_ROBIN,
    SCHEDULER_WORK_STEALING
} SchedulerType;


// Scheduler Implementations
void job_scheduler_round_robin_init(int worker_count);
//void job_scheduler_work_stealing_init();

JobScheduler* job_scheduler_init(int requested_worker_count, SchedulerType type);
void job_scheduler_shutdown();

#endif // JOB_SCHEDULER_H
