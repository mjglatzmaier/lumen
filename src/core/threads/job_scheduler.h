#ifndef JOB_SCHEDULER_H
#define JOB_SCHEDULER_H

#include "job_queue.h"

typedef struct JobScheduler {
    void (*init)();                        // Initialize scheduler
    void (*submit_job)(Job *job);          // Submit a job
    void (*shutdown)();                    // Shut down scheduler
} JobScheduler;

// Expose a global scheduler instance
extern JobScheduler *scheduler;

// Scheduler implementations
//void job_scheduler_round_robin_init();
//void job_scheduler_work_stealing_init();

void job_scheduler_init();
void job_scheduler_submit(Job *job);
void job_scheduler_shutdown();

#endif // JOB_SCHEDULER_H
