#include "containers/cont_da.h"
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>
#include "../test_framework.h"

// Test dynamic array functionality
bool test_da_default_alloc() {
    // Instantiate an alloc
    lum_allocator* allocator = (lum_allocator*)lum_create_default_allocator();
    int* array = lum_da_create(int, 0, allocator);

    // Check empty state
    assert(lum_da_size(array) == 0);

    // Push values
    for (int i = 0; i < 10; i++) {
        lum_da_app(array, i);
        assert(array != NULL);
        assert(lum_da_size(array) == i + 1);
    }

    // Validate stored values
    for (int i = 0; i < 10; i++) {
        assert(array[i] == i);
    }

    // Free memory
    lum_da_free(array);
    return true;
}

// Test large allocations
bool test_da_large_alloc() {
    lum_allocator* allocator = (lum_allocator*)lum_create_default_allocator();
    int* array = lum_da_create(int, 0, allocator);
    const int N = 1000000;

    for (int i = 0; i < N; i++) {
        lum_da_app(array, i);
        assert(array != NULL);
    }

    assert(lum_da_size(array) == N);
    lum_da_free(array);
    return true;
}

// Test clearing an array
bool test_lum_da_clear() {
    lum_allocator* allocator = (lum_allocator*)lum_create_default_allocator();
    int* array = lum_da_create(int, 0, allocator);
    int val = 42;

    for (int i = 0; i < 100; i++) {
        lum_da_app(array, val);
    }

    assert(lum_da_size(array) == 100);
    lum_da_free(array);
    assert(lum_da_size(array) == 0);
    return true;
}


// **Define test cases**
TestCase cont_da_tests[] = {
    {"test_da_default_alloc", test_da_default_alloc},
    {"test_da_large_alloc", test_da_large_alloc},
    {"test_lum_da_clear", test_lum_da_clear},
};

// **Test runner function**
int cont_da_tests_count = sizeof(cont_da_tests) / sizeof(TestCase);

