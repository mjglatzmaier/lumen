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
    int* array = cont_da_create(int, 0, allocator);

    // Check empty state
    assert(cont_da_size(array) == 0);
    //assert(lum_da_capacity(array) == 0);

    // Push values
    for (int i = 0; i < 10; i++) {
        cont_da_app(array, i);
        assert(array != NULL);
        assert(cont_da_size(array) == i + 1);
    }

    // Validate stored values
    for (int i = 0; i < 10; i++) {
        assert(array[i] == i);
    }

    // Remove last element
    //lum_da_pop(array);
    //assert(cont_da_count(array) == 9);

    // Free memory
    cont_da_free(array);
    return true;
}

// Test large allocations
bool test_da_large_alloc() {
    lum_allocator* allocator = (lum_allocator*)lum_create_default_allocator();
    int* array = cont_da_create(int, 0, allocator);
    const int N = 1000000;

    for (int i = 0; i < N; i++) {
        cont_da_app(array, i);
        assert(array != NULL);
    }

    assert(cont_da_size(array) == N);
    cont_da_free(array);
    return true;
}

// Test clearing an array
bool test_lum_da_clear() {
    lum_allocator* allocator = (lum_allocator*)lum_create_default_allocator();
    int* array = cont_da_create(int, 0, allocator);
    int val = 42;

    for (int i = 0; i < 100; i++) {
        cont_da_app(array, val);
    }

    assert(cont_da_size(array) == 100);
    cont_da_free(array);
    assert(cont_da_size(array) == 0);
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

