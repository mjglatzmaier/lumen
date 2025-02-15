#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>
#include "job_scheduler.h"
#include "platform.h"
#include "../test_framework.h"

#define NUM_TEST_JOBS 100000
#define WORKER_COUNT 4  // Must match the definition in job_scheduler.c

atomic_int jobs_executed = 0;
atomic_int worker_distribution[WORKER_COUNT] = {0};  // Track job execution per worker

void test_job_function(void *arg) {
    int worker_id = *(int *)arg;
    atomic_fetch_add(&jobs_executed, 1);
    atomic_fetch_add(&worker_distribution[worker_id], 1);
}

void calc_pi(void *arg) {
    int num_samples = *(int *)arg;
    double sum = 0.0;

    for (int i = 0; i < num_samples; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1.0) sum += 1.0;
    }
    double result = (sum / num_samples) * 4.0;
}

// Start worker threads that submit jobs concurrently
void *worker_submit_jobs(void *arg)
{
    for (int i = 0; i < NUM_TEST_JOBS / WORKER_COUNT; i++)
    {
        Job *job = malloc(sizeof(Job));
        job->function = test_job_function;
        job->data = malloc(sizeof(int));
        *(int *)job->data = *(int *)arg;
        job_scheduler_submit(job);
    }
    return NULL;
}
/**
 * Test 1: Basic Job Execution
 * Ensures all jobs are executed.
 */
static bool test_basic_execution() {
    jobs_executed = 0;
    job_scheduler_init();

    for (int i = 0; i < NUM_TEST_JOBS; i++) {
        Job *job = malloc(sizeof(Job));
        job->function = test_job_function;
        job->data = malloc(sizeof(int));
        *(int *)job->data = i % WORKER_COUNT;  // Simulate worker ID assignment
        job_scheduler_submit(job);
    }
    job_scheduler_shutdown();

    assert(jobs_executed == NUM_TEST_JOBS && "Failed to execute all jobs!");
    return true;
}

/**
 * Test 2: Load Balancing Verification
 * Ensures jobs are evenly distributed across workers.
 */
static bool test_load_balancing() {
    for (int i = 0; i < WORKER_COUNT; i++) {
        atomic_store(&worker_distribution[i], 0);
    }

    job_scheduler_init();

    for (int i = 0; i < NUM_TEST_JOBS; i++) {
        Job *job = malloc(sizeof(Job));
        job->function = test_job_function;
        job->data = malloc(sizeof(int));
        *(int *)job->data = i % WORKER_COUNT;
        job_scheduler_submit(job);
    }

    job_scheduler_shutdown();

    int avg_jobs = NUM_TEST_JOBS / WORKER_COUNT;
    int imbalance = 0;
    
    for (int i = 0; i < WORKER_COUNT; i++) {
        int executed = atomic_load(&worker_distribution[i]);
        printf(" - Worker %d: %d jobs (expected ~%d)\n", i, executed, avg_jobs);
        imbalance += abs(executed - avg_jobs);
    }

    assert(imbalance < avg_jobs * 0.1 && "Failed: Significant imbalance detected in job distribution.");
    return true;
}

/**
 * Test 3: Performance Benchmarking
 * Measures job execution throughput.
 */
static bool test_performance() {
    jobs_executed = 0;
    job_scheduler_init();

    for (int i = 0; i < NUM_TEST_JOBS; i++) {
        Job *job = malloc(sizeof(Job));
        job->function = calc_pi;
        job->data = malloc(sizeof(int));
        *(int *)job->data = 10000000;
        job_scheduler_submit(job);
    }
    job_scheduler_shutdown();
    assert(jobs_executed == NUM_TEST_JOBS && "Failed: Some jobs were lost due to potential race conditions.");

    return true;
}

/**
 * Test 4: Thread Safety (Race Condition Test)
 * Verifies that job execution is thread-safe by submitting many jobs concurrently.
 */
static bool test_thread_safety() {
    jobs_executed = 0;
    lum_thread threads[WORKER_COUNT];

    job_scheduler_init();

    for (int i = 0; i < WORKER_COUNT; i++) {
        threads[i] = lum_create_thread(worker_submit_jobs, &i);
    }

    for (int i = 0; i < WORKER_COUNT; i++) {
        lum_thread_join(threads[i]);
    }
    job_scheduler_shutdown();

    assert(jobs_executed == NUM_TEST_JOBS && "Failed: Some jobs were lost due to potential race conditions.");
    return true;
}

//  Register Tests
TestCase job_scheduling_tests[] = {
    DECLARE_TEST(test_basic_execution),
    DECLARE_TEST(test_load_balancing),
    DECLARE_TEST(test_performance),
    DECLARE_TEST(test_thread_safety)
};

int job_scheduling_tests_count = sizeof(job_scheduling_tests) / sizeof(TestCase);