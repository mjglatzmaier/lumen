#include "../test_framework.h"
#include "containers/cont_da.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// Test dynamic array functionality
bool test_da_default_alloc()
{
    // Instantiate an alloc
    lum_allocator *allocator = (lum_allocator *) lum_create_default_allocator();
    lum_da *array = lum_da_create(sizeof(int), 0, allocator);

    // Check empty state
    assert(lum_da_size(array) == 0);

    // Push values
    for (int i = 0; i < 10; i++)
    {
        lum_da_app(array, i);
        assert(array != NULL);
        //assert(lum_da_size(array) == i + 1);
    }

    // Validate stored values
    for (int i = 0; i < 10; i++)
    {
        assert(lum_da_data(array, int)[i] == i);
    }

    // Free memory
    lum_da_free(array);

    return true;
}

// Test large allocations
bool test_da_large_alloc()
{
    lum_allocator *allocator = (lum_allocator *) lum_create_default_allocator();
    lum_da *array     = lum_da_create(sizeof(int), 0, allocator);
    const int      N         = 1000000;

    for (int i = 0; i < N; i++)
    {
        lum_da_app(array, i);
        assert(array != NULL);
    }

    //assert(lum_da_size(array) == N);
    lum_da_free(array);
    return true;
}

// Test clearing an array
bool test_lum_da_clear()
{
    lum_allocator *allocator = (lum_allocator *) lum_create_default_allocator();
    lum_da *array = lum_da_create(sizeof(int), 0, allocator);
    int val       = 42;

    for (int i = 0; i < 100; i++)
    {
        lum_da_app(array, i);
    }

    //assert(lum_da_size(array) == 100);
    lum_da_free(array);
    //assert(lum_da_size(array) == 0);
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
