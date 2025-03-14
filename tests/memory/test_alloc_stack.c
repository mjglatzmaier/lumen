#include "../test_framework.h"
#include "allocators/mem_alloc.h"
#include "allocators/mem_stack.h"

#include <stdint.h>
#include <stdio.h>

#define STACK_SIZE 1024 // 1 KB for testing
#define ALIGNMENT 16    // Ensure SIMD alignment

// Test: Basic Stack Allocation
bool test_stack_alloc_basic(void)
{
    lum_allocator *stack_alloc = lum_create_stack_allocator(STACK_SIZE);
    ASSERT_NOT_NULL(stack_alloc);

    void *ptr1 = lum_stack_alloc(stack_alloc, 128, ALIGNMENT);
    void *ptr2 = lum_stack_alloc(stack_alloc, 256, ALIGNMENT);

    ASSERT_NOT_NULL(ptr1);
    ASSERT_NOT_NULL(ptr2);

    // Ensure pointers are different (not overlapping)
    ASSERT_TRUE(ptr1 != ptr2);

    lum_destroy_stack_allocator(stack_alloc);
    return true;
}

// Test: Stack Reset
bool test_stack_alloc_reset(void)
{
    lum_allocator *stack_alloc = lum_create_stack_allocator(STACK_SIZE);
    ASSERT_NOT_NULL(stack_alloc);

    void *ptr1 = lum_stack_alloc(stack_alloc, 128, ALIGNMENT);
    ASSERT_NOT_NULL(ptr1);

    lum_stack_reset(stack_alloc); // Reset the stack

    void *ptr2 = lum_stack_alloc(stack_alloc, 128, ALIGNMENT);
    ASSERT_NOT_NULL(ptr2);

    // After reset, we should get the same pointer back
    ASSERT_TRUE(ptr1 == ptr2);

    lum_destroy_stack_allocator(stack_alloc);
    return true;
}

// Test: Alignment Check
bool test_stack_alloc_alignment(void)
{
    lum_allocator *stack_alloc = lum_create_stack_allocator(STACK_SIZE);
    ASSERT_NOT_NULL(stack_alloc);

    void *ptr = lum_stack_alloc(stack_alloc, 64, ALIGNMENT);
    ASSERT_NOT_NULL(ptr);

    // Ensure proper alignment
    ASSERT_TRUE(((uintptr_t) ptr % ALIGNMENT) == 0);

    lum_destroy_stack_allocator(stack_alloc);
    return true;
}

bool test_stack_alloc_pop(void)
{
    lum_allocator *stack_alloc = lum_create_stack_allocator(STACK_SIZE);
    ASSERT_NOT_NULL(stack_alloc);

    void *p1 = lum_stack_alloc(stack_alloc, 64, 16);
    void *p2 = lum_stack_alloc(stack_alloc, 32, 16);
    ASSERT_NOT_NULL(p1);
    ASSERT_NOT_NULL(p2);

    lum_stack_pop(stack_alloc, p2);                  // Pop last allocation
    void *p3 = lum_stack_alloc(stack_alloc, 32, 16); // Should reuse same space
    ASSERT_TRUE(p3 == p2);                           // Pop should have freed the last allocation

    lum_destroy_stack_allocator(stack_alloc);
    return true;
}

// **Define test cases**
TestCase stack_alloc_tests[] = {
    {"test_stack_alloc_basic", test_stack_alloc_basic},
    {"test_stack_alloc_reset", test_stack_alloc_reset},
    {"test_stack_alloc_alignment", test_stack_alloc_alignment},
    {"test_stack_alloc_pop", test_stack_alloc_pop},
};

// **Test runner function**
int stack_alloc_tests_count = sizeof(stack_alloc_tests) / sizeof(TestCase);
