#include "../memory/allocators/mem_alloc.h"
#include "../math/math_rand.h"
#include "../test_framework.h"
#include "lum_scheduler.h"
#include "lum_thread.h"
#include "platform.h"

#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TEST_NUM_JOBS 64
#define NUM_THREADS 8
#define PI_ITERATIONS 1000000

atomic_int fast_counter  = 0;
atomic_int heavy_counter = 0;

typedef struct {
    int id;
    float result;
} jobArg;

// Simple job function that increments the counter
void *fast_job(void *arg)
{
    (void) arg; // Unused
    atomic_fetch_add(&fast_counter, 1);
    return NULL;
}

// Heavy compute job: Monte Carlo Pi estimation
static void *heavy_job(void *arg)
{
    jobArg *ja = (jobArg*) arg;
    if (!ja) 
        return NULL;

    pcg32_random_t rng = { .state = ja->id * 123456789u, .inc = (ja->id * 987654321u) | 1 };
    int inside_circle = 0;
    
    for (int i = 0; i < PI_ITERATIONS; i++)
    {
        double x = (double) pcg32_random_r(&rng) / UINT32_MAX;
        double y = (double) pcg32_random_r(&rng) / UINT32_MAX;
        if (x * x + y * y <= 1.0)
            inside_circle++;
    }

    ja->result = (4.0 * inside_circle) / PI_ITERATIONS;
    atomic_fetch_add(&heavy_counter, 1);
    return NULL;
}

// Unit test: Check if jobs execute correctly
static bool test_scheduler_basic_execution()
{

    // Reset counter
    atomic_store(&fast_counter, 0);

    // Create scheduler configuration
    lum_scheduler_config_t config = {0};
    config.num_threads            = 2;
    config.queue_capacity         = TEST_NUM_JOBS; // Always include an empty slot

    lum_scheduler_t *scheduler = lum_scheduler_create(&config);
    assert(scheduler != NULL && "Scheduler creation failed!");

    // Submit jobs
    for (int i = 0; i < TEST_NUM_JOBS; i++)
    {
        //  Note: the scheduler will free these job pointers
        //  after execution, so we *MUST* allocate them using the scheduler's allocator.
        Job *job = lum_scheduler_create_job(scheduler, fast_job, (void *) (intptr_t) i);
        lum_scheduler_submit(scheduler, job);
    }

    // Give some time for the jobs to execute
    lum_thread_sleep(100);

    // Shutdown the scheduler
    lum_scheduler_destroy(scheduler);

    // Validate that all jobs executed
    assert(atomic_load(&fast_counter) == TEST_NUM_JOBS && "Not all jobs executed!");
    return true;
}

// Unit test: Test multiple jobs with multi-threading
static bool test_scheduler_multithreading()
{

    atomic_store(&fast_counter, 0);

    // Create scheduler configuration
    lum_scheduler_config_t config = {0};
    config.num_threads            = 4;
    config.queue_capacity         = TEST_NUM_JOBS * 2 + 1; // Always include an empty slot

    lum_scheduler_t *scheduler = lum_scheduler_create(&config);
    assert(scheduler != NULL && "Scheduler creation failed!");

    // Submit jobs
    for (int i = 0; i < TEST_NUM_JOBS * 2; i++)
    {
        // Note: the scheduler will free these job pointers
        // after execution, so we *MUST* allocate them using the scheduler's allocator.
        Job *job = lum_scheduler_create_job(scheduler, fast_job, (void *) (intptr_t) i);
        lum_scheduler_submit(scheduler, job);
    }

    // Wait for jobs to complete
    lum_thread_sleep(200);

    // Shutdown the scheduler
    lum_scheduler_destroy(scheduler);

    // Validate that all jobs executed
    assert(atomic_load(&fast_counter) == TEST_NUM_JOBS * 2 && "Not all jobs executed!");

    return true;
}

static bool test_scheduler_stress()
{
    lum_scheduler_config_t config = {0};
    config.num_threads            = NUM_THREADS;
    config.queue_capacity         = TEST_NUM_JOBS;
    lum_scheduler_t *scheduler    = lum_scheduler_create(&config);
    assert(scheduler != NULL && "Scheduler creation failed!");
    atomic_store(&fast_counter, 0);
    atomic_store(&heavy_counter, 0);

    // Track start time
    clock_t start             = clock();
    int     fast_submissions  = 0;
    int     heavy_submissions = 0;

    jobArg args[TEST_NUM_JOBS];

    // Submit jobs
    for (int i = 0; i < TEST_NUM_JOBS; i++)
    {
        args[i].id = i % NUM_THREADS;
        args[i].result = 0;
        void *arg = (void *) &args[i];
        Job  *job;

        if (rand() % 2 == 0)
        { // 50% chance for each job type
            ++fast_submissions;
            job = lum_scheduler_create_job(scheduler, fast_job, arg);
        }
        else
        {
            ++heavy_submissions;
            job = lum_scheduler_create_job(scheduler, heavy_job, arg);
        }
        lum_scheduler_submit(scheduler, job);
    }
    
    lum_scheduler_wait_completion(scheduler);
    
    lum_scheduler_destroy(scheduler);
    
    float pi_avg = 0;
    for (int i = 0; i < TEST_NUM_JOBS; i++) {
        if (args[i].result != 0) {
            pi_avg += args[i].result;
        }
    }
    pi_avg /= atomic_load(&heavy_counter);
    printf("pi avg=%.6f\n", pi_avg);

    // Track end time
    clock_t end        = clock();
    double  time_spent = (double) (end - start) / CLOCKS_PER_SEC;

    printf(" Fast jobs completed: %d, submitted: %d\n", atomic_load(&fast_counter),
           fast_submissions);
    printf(" Heavy jobs completed: %d, submitted: %d\n", atomic_load(&heavy_counter),
           heavy_submissions);
    printf("⏱ Total execution time: %.3f seconds\n", time_spent);

    return true;
}

// **Define test cases**
TestCase lum_scheduler_tests[] = {
    {"test_scheduler_basic_execution", test_scheduler_basic_execution},
    {"test_scheduler_multithreading", test_scheduler_multithreading},
    {"test_scheduler_stress", test_scheduler_stress}};

// **Test runner function**
int lum_scheduler_tests_count = sizeof(lum_scheduler_tests) / sizeof(TestCase);