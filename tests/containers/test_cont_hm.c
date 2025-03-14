#include "../test_framework.h"
#include "containers/cont_hm.h"

#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_KEYS 1000000  // 1 Million Keys for Benchmarking


bool lum_hm_basic_ops() {
    lum_allocator *allocator = lum_create_default_allocator();
    lum_hm *hm = lum_hm_create(16, lum_hash_metro, allocator);
    assert(hm != NULL);

    // Insert values
    int key1 = 42, key2 = 100, key3 = 7;
    const char *val1 = "Player", *val2 = "Enemy", *val3 = "NPC";

    lum_hm_put(hm, key1, val1);
    lum_hm_put(hm, key2, val2);
    lum_hm_put(hm, key3, val3);

    // Verify lookups
    char * result;
    assert(lum_hm_get(hm, key1, result) == 1);
    assert(lum_hm_get(hm, key2, result) == 1);
    assert(lum_hm_get(hm, key3, result) == 1);
    
    // Test overwriting an entry
    const char *new_val = "Boss";
    lum_hm_put(hm, key2, new_val);
    assert(lum_hm_get(hm, key2, result) == 1);

    // Remove key and check if it’s gone
    lum_hm_remove(hm, key1);

    assert(lum_hm_get(hm, key1, result) == 0);

    // Free hashmap
    lum_hm_free(hm);
    return true;
}

bool lum_hm_benchmark() {
    lum_allocator *allocator = lum_create_default_allocator();
    lum_hm *hm = lum_hm_create(4 * NUM_KEYS, lum_hash_xxhash, allocator);
    int *keys = allocator->alloc(allocator, NUM_KEYS * sizeof(int), 16);
    int *values = allocator->alloc(allocator, NUM_KEYS * sizeof(int), 16);

    // Generate keys and values
    for (int i = 0; i < NUM_KEYS; i++) {
        keys[i] = i;
        values[i] = i * 2;  // Arbitrary value
    }
    clock_t start = clock();
    // Benchmark Insertions
    for (int i = 0; i < NUM_KEYS; i++) {
        lum_hm_put(hm, keys[i], values[i]);
    }
    // Benchmark Retrieval (Existing Keys)
    for (int i = 0; i < NUM_KEYS; i++) {
        int retrieved;
        bool found = lum_hm_get(hm, keys[i], retrieved);
        assert(found && retrieved == values[i]);
    }
    // Benchmark Retrieval (Missing Keys)
    for (int i = NUM_KEYS; i < 2 * NUM_KEYS; i++) {
        int retrieved;
        bool found = lum_hm_get(hm, i, retrieved);
        assert(!found);
    }
    // Benchmark Removals
    for (int i = 0; i < NUM_KEYS; i++) {
        lum_hm_remove(hm, keys[i]);
    }
    clock_t end = clock();
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("⏱️  Insertions, Retrievals, and Removals took %.3f seconds\n", elapsed_time);

    allocator->free(allocator, keys);
    allocator->free(allocator, values);
    lum_hm_free(hm); // frees allocator too

    return true;
}

// **Define test cases**
TestCase cont_hm_tests[] = {
    {"test_da_default_alloc", lum_hm_basic_ops},
    {"test_lum_hm_benchmark", lum_hm_benchmark},
};

// **Test runner function**
int cont_hm_tests_count = sizeof(cont_hm_tests) / sizeof(TestCase);