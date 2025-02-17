#ifndef LUMEN_WORKER_THREAD_H
#define LUMEN_WORKER_THREAD_H

#include "../platform.h"

#include <stdatomic.h>
#include <stdbool.h>

typedef struct lum_allocator lum_allocator;

typedef struct Job
{
    lum_thread_func function;
    void           *data;
    atomic_int      remaining_dependencies;
} Job;

typedef enum
{
    LUM_THREAD_TASK,
    LUM_THREAD_NUMA
} lum_thread_type;

typedef struct
{
    lum_thread_type type;
    lum_thread      thread;
    int             numa_node; // Only used for NUMA threads
    atomic_bool     running;
    // lum_allocator* allocator; // unused?
} lum_thread_t;

void lum_thread_init(lum_thread_t *worker, int id, lum_thread_func func, void *arg);
void lum_thread_shutdown(lum_thread_t *worker);

#endif
