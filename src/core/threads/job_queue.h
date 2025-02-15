#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include <stdatomic.h>

typedef void (*JobFunction)(void *data);

#define WORKER_COUNT 4
#define JOB_QUEUE_SIZE 1024

typedef struct WorkerThread WorkerThread;

typedef struct Job {
    JobFunction function;
    void *data;
    atomic_int remaining_dependencies;
} Job;

typedef struct JobQueue {
    Job *jobs[JOB_QUEUE_SIZE];
    atomic_int top;
    atomic_int bottom;
} JobQueue;

//void job_system_init();
//void submit_job(Job *job);
void push_job(JobQueue *queue, Job *job);
Job *pop_job(JobQueue *queue);
Job *steal_job(JobQueue *queue);
//void wait_for_job(Job *job);
//void execute_job(Job *job);

#endif // JOB_QUEUE_H
