#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#ifdef USE_MIMALLOC
    #include <mimalloc.h>
#endif
#include "../test_framework.h"
#include "allocators/mem_alloc.h"

#define NUM_ALLOCATIONS 1000000
#define MIN_SIZE 16
#define MAX_SIZE 1024

// **Benchmarking default malloc/free**
static bool benchmark_default_alloc() {
    void* ptrs[NUM_ALLOCATIONS];
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        size_t size = (rand() % (MAX_SIZE - MIN_SIZE + 1)) + MIN_SIZE;
        ptrs[i] = malloc(size);
        if (!ptrs[i]) {
            printf("malloc failed at iteration %d\n", i);
            exit(1);
        }
    }
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        free(ptrs[i]);
    }
    return true;
}

// **Benchmarking mimalloc**
static bool benchmark_mimalloc() {
    #ifdef USE_MIMALLOC
    void* ptrs[NUM_ALLOCATIONS];
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        size_t size = (rand() % (MAX_SIZE - MIN_SIZE + 1)) + MIN_SIZE;
        ptrs[i] = mi_malloc(size);
        if (!ptrs[i]) {
            printf("mimalloc failed at iteration %d\n", i);
            exit(1);
        }
    }
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        mi_free(ptrs[i]);
    }
    #endif
    return true;
}

// **Test Suite**
TestCase mimalloc_tests[] = {
    DECLARE_TEST(benchmark_default_alloc),
    DECLARE_TEST(benchmark_mimalloc),
};

// **Define the count explicitly**
int mimalloc_tests_count = sizeof(mimalloc_tests) / sizeof(TestCase);