#include "allocators/mem_pool.h"
#include "../test_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_OBJECTS 10000
#define TEST_ALIGNMENT 16

typedef struct {
    int x, y, z;
} Point3D;

// Benchmark helper
static double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000.0) + (ts.tv_nsec / 1000000.0);
}


// Test 1: Basic allocation & reset
static bool test_pool_allocator() {
    // Create a pool allocator for Point3D structs
    lum_allocator* pool_alloc = lum_create_pool_allocator(sizeof(Point3D), NUM_OBJECTS);
    assert(pool_alloc && "Failed to create pool allocator!");

    Point3D* objects[NUM_OBJECTS];

    // Step 1: Allocate all objects
    for (size_t i = 0; i < NUM_OBJECTS; i++) {
        objects[i] = pool_alloc->alloc(pool_alloc, sizeof(Point3D), 16);
        assert(objects[i] != NULL && "Allocation failed!");
        assert(((uintptr_t)objects[i] & (TEST_ALIGNMENT - 1)) == 0 && "Pointer misaligned!");
    }

    // Step 2: Ensure uniqueness of allocated pointers
    for (size_t i = 0; i < NUM_OBJECTS - 1; i++) {
        for (size_t j = i + 1; j < NUM_OBJECTS; j++) {
            assert(objects[i] != objects[j] && "Duplicate pointer allocated!");
        }
    }

    // Step 3: Free half the objects
    for (size_t i = 0; i < NUM_OBJECTS / 2; i++) {
        pool_alloc->free(pool_alloc, objects[i]);
    }

    // Step 4: Reallocate the freed objects
    Point3D* new_objects[NUM_OBJECTS / 2];
    for (size_t i = 0; i < NUM_OBJECTS / 2; i++) {
        new_objects[i] = pool_alloc->alloc(pool_alloc, sizeof(Point3D), 16);
        assert(new_objects[i] != NULL && "Reallocation failed!");
    }

    // Step 5: Ensure LIFO reuse
    for (size_t i = 0; i < NUM_OBJECTS / 2; i++) {
        assert(new_objects[i] == objects[(NUM_OBJECTS / 2) - 1 - i]&&
               "Pool allocator is not reusing freed memory in LIFO order!");
    }

    // Cleanup
    lum_pool_allocator_destroy(pool_alloc);
    return true;
}

// Test 2: Allocating beyond frame limits
static bool benchmark_pool_vs_malloc() {

    Point3D* objects[NUM_OBJECTS];

    // Benchmark Pool Allocator
    lum_allocator* pool_alloc = lum_create_pool_allocator(sizeof(Point3D), NUM_OBJECTS);
    assert(pool_alloc);// && "Failed to create pool allocator!");

    double start_time = get_time_ms();
    for (size_t i = 0; i < NUM_OBJECTS; i++) {
        objects[i] = pool_alloc->alloc(pool_alloc, sizeof(Point3D), 16);
    }
    double pool_alloc_time = get_time_ms() - start_time;

    start_time = get_time_ms();
    for (size_t i = 0; i < NUM_OBJECTS; i++) {
        pool_alloc->free(pool_alloc, objects[i]);
    }
    double pool_free_time = get_time_ms() - start_time;

    lum_pool_allocator_destroy(pool_alloc);

    // Benchmark malloc/free
    start_time = get_time_ms();
    for (size_t i = 0; i < NUM_OBJECTS; i++) {
        objects[i] = (Point3D*)malloc(sizeof(Point3D));
    }
    double malloc_alloc_time = get_time_ms() - start_time;

    start_time = get_time_ms();
    for (size_t i = 0; i < NUM_OBJECTS; i++) {
        free(objects[i]);
    }
    double malloc_free_time = get_time_ms() - start_time;
    printf("\n");
    printf("Pool Allocator - Alloc: %.3f ms, Free: %.3f ms\n", pool_alloc_time, pool_free_time);
    printf("malloc/free - Alloc: %.3f ms, Free: %.3f ms\n", malloc_alloc_time, malloc_free_time);
    printf("\n");
    return true;
}

//  Register Tests
TestCase pool_alloc_tests[] = {
    DECLARE_TEST(test_pool_allocator),
    DECLARE_TEST(benchmark_pool_vs_malloc)
};

int pool_alloc_tests_count = sizeof(pool_alloc_tests) / sizeof(TestCase);
