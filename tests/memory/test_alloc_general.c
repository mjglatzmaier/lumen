#include "../test_framework.h"
#include "allocators/mem_alloc.h"
#include <stdio.h>
#include <time.h>

#define ALLOC_SIZE 1024
#define ALIGNMENT 16
#define NUM_ITERATIONS 1000000

//  Benchmark helper
static void run_alloc(lum_allocator* allocator, size_t size, size_t alignment, int iterations) {    
    for (int i = 0; i < iterations; i++) {
        void* ptr = allocator->alloc(allocator, size, alignment);
        allocator->free(allocator, ptr);
    }
}

// Test: Basic Allocation
static bool test_general_alloc_basic() {
    lum_allocator* allocator = lum_get_default_allocator();

    void* ptr = allocator->alloc(allocator, ALLOC_SIZE, ALIGNMENT);
    if (!ptr) return false;  // Allocation failed

    allocator->free(allocator, ptr);
    return true;
}

// Test: Reallocation
static bool test_general_realloc() {
    lum_allocator* allocator = lum_get_default_allocator();

    void* ptr = allocator->alloc(allocator, ALLOC_SIZE, ALIGNMENT);
    if (!ptr) return false;

    void* new_ptr = allocator->realloc(allocator, ptr, ALLOC_SIZE * 2);
    if (!new_ptr) return false;

    allocator->free(allocator, new_ptr);
    return true;
}

// Benchmark: Default
static bool benchmark_general_alloc() {
    lum_allocator* allocator = lum_get_default_allocator();
    run_alloc(allocator, ALLOC_SIZE, ALIGNMENT, NUM_ITERATIONS);
    return true;
}

// **Test Suite**
TestCase gen_alloc_tests[] = {
    DECLARE_TEST(test_general_alloc_basic),
    DECLARE_TEST(test_general_realloc),
    DECLARE_TEST(benchmark_general_alloc),
    DECLARE_TEST(test_general_realloc),
};

// **Define the count explicitly**
int gen_alloc_tests_count = sizeof(gen_alloc_tests) / sizeof(TestCase);