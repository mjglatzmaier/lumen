#include "../test_framework.h"
#include "allocators/mem_alloc.h"
#include "containers/cont_lfq.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define TEST_CAPACITY 1024
#define NUM_OPERATIONS 1000000 // For benchmark testing

// Simple integer wrapper for testing
typedef struct
{
    int value;
} TestItem;

// Test single-threaded enqueue & dequeue
static bool test_basic_operations()
{
    // Allocate queue
    lum_allocator *allocator = lum_create_default_allocator();
    assert(allocator != NULL);
    lum_lfq_t *queue = allocator->alloc(allocator, sizeof(lum_lfq_t), 16);
    lum_lfq_init(queue, TEST_CAPACITY, allocator);

    TestItem item1 = {42};
    TestItem item2 = {99};

    // Queue should be empty
    TestItem *deq = (TestItem *) lum_lfq_dequeue(queue);
    assert(deq == NULL);

    // Enqueue item1
    assert(lum_lfq_enqueue(queue, &item1) == 1);
    // Enqueue item2
    assert(lum_lfq_enqueue(queue, &item2) == 1);

    // Dequeue and check values
    TestItem *deq1 = (TestItem *) lum_lfq_dequeue(queue);
    assert(deq1->value == 42);

    TestItem *deq2 = (TestItem *) lum_lfq_dequeue(queue);
    assert(deq2->value == 99);

    // Queue should be empty again
    TestItem *deq3 = (TestItem *) lum_lfq_dequeue(queue);
    assert(deq3 == NULL);

    lum_lfq_free(&queue);
    assert(queue == NULL);

    return true;
}

// Test queue behavior when full
static bool test_queue_full()
{
    // Allocate queue
    lum_allocator *allocator = lum_create_default_allocator();
    assert(allocator != NULL);
    lum_lfq_t *queue = allocator->alloc(allocator, sizeof(lum_lfq_t), 16);
    lum_lfq_init(queue, TEST_CAPACITY, allocator);

    TestItem items[TEST_CAPACITY];
    for (int i = 0; i < TEST_CAPACITY; i++)
    {
        items[i].value = i;
        assert(lum_lfq_enqueue(queue, &items[i]));
    }

    // Try enqueueing one more item (should fail)
    TestItem extra = {999};
    assert(!lum_lfq_enqueue(queue, &extra));

    // Dequeue and validate order
    for (int i = 0; i < TEST_CAPACITY; i++)
    {
        TestItem *item = (TestItem *) lum_lfq_dequeue(queue);
        assert(item->value == i);
    }

    // Queue should be empty now
    TestItem *deq = (TestItem *) lum_lfq_dequeue(queue);
    assert(deq == NULL);

    lum_lfq_free(&queue);
    assert(queue == NULL);

    return true;
}

// Benchmark performance
static bool benchmark_queue()
{
    // Allocate queue
    lum_allocator *allocator = lum_create_default_allocator();
    assert(allocator != NULL);
    lum_lfq_t *queue = allocator->alloc(allocator, sizeof(lum_lfq_t), 16);
    lum_lfq_init(queue, TEST_CAPACITY, allocator);

    TestItem items[NUM_OPERATIONS];

    // Enqueue operations
    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        items[i % TEST_CAPACITY].value = i;
        lum_lfq_enqueue(queue, &items[i % TEST_CAPACITY]);
    }

    // Dequeue operations
    for (int i = 0; i < NUM_OPERATIONS; i++)
    {
        TestItem *item = (TestItem *) lum_lfq_dequeue(queue);
    }

    // Queue should be empty now
    TestItem *deq = (TestItem *) lum_lfq_dequeue(queue);
    assert(deq == NULL);

    lum_lfq_free(&queue);
    assert(queue == NULL);

    return true;
}

// **Define test cases**
TestCase cont_lfq_tests[] = {
    {"test_basic_operations", test_basic_operations},
    {"test_queue_full", test_queue_full},
    {"benchmark_queue", benchmark_queue},
};

// **Test runner function**
int cont_lfq_tests_count = sizeof(cont_lfq_tests) / sizeof(TestCase);
