
#include "../test_framework.h"
#include "allocators/mem_alloc.h"
#include "containers/cont_lfq.h"
#include "platform.h"

#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_CAPACITY 1024
#define NUM_THREADS 8
#define NUM_OPERATIONS 1000000

typedef struct
{
    int value;
} TestItem;

typedef struct
{
    lum_lfq_t     *queue;
    int            thread_id;
    lum_allocator *allocator;
    int            num_items;
} ThreadArgs;

atomic_int jobs_executed = 0;

void *producer_thread(void *arg)
{
    ThreadArgs *args = (ThreadArgs *) arg;
    for (int i = 0; i < args->num_items; i++)
    {
        TestItem *item = args->allocator->alloc(args->allocator, sizeof(TestItem), 16);
        item->value    = i + (args->thread_id * args->num_items);
        lum_lfq_enqueue(args->queue, item);
    }
    return NULL;
}

void *consumer_thread(void *arg)
{
    ThreadArgs *args = (ThreadArgs *) arg;
    TestItem   *item;
    while ((item = (TestItem *) lum_lfq_dequeue(args->queue)) || !lum_lfq_empty(args->queue))
    {
        if (item)
        {
            assert(item != NULL);
            args->allocator->free(args->allocator, item);
            atomic_fetch_add(&jobs_executed, 1);
        }
    }
    return NULL;
}

static bool test_split_production_consumption()
{
    // Allocate queue
    lum_allocator *allocator = lum_create_default_allocator();
    assert(allocator != NULL);

    lum_lfq_t *queue = allocator->alloc(allocator, sizeof(lum_lfq_t), 16);
    lum_lfq_init(queue, TEST_CAPACITY, allocator);

    lum_thread producers[NUM_THREADS];
    pthread_t  consumers[NUM_THREADS];
    ThreadArgs args[NUM_THREADS];

    // Launch producer threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        args[i].queue     = queue;
        args[i].allocator = allocator;
        args[i].thread_id = i;
        args[i].num_items = NUM_OPERATIONS / NUM_THREADS;
        producers[i]      = lum_thread_create(producer_thread, (void *) &args[i]);
    }

    // Wait for producers to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        lum_thread_join(producers[i]);
    }

    // Launch consumer threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        consumers[i] = lum_thread_create(consumer_thread, (void *) &args[i]);
    }
    // Wait for consumers to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        lum_thread_join(consumers[i]);
    }

    // Verify that all jobs were executed
    assert(jobs_executed == NUM_OPERATIONS &&
           "Failed: Some jobs were lost due to potential race conditions.");

    // Clean up
    lum_lfq_free(queue);
    return true;
}

// **Define test cases**
TestCase cont_lfq_mt_tests[] = {
    {"test_split_production_consumption", test_split_production_consumption}};

// **Test runner function**
int cont_lfq_mt_tests_count = sizeof(cont_lfq_mt_tests) / sizeof(TestCase);