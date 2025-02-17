#include "../test_framework.h"
#include "allocators/mem_frame.h"

#include <stdint.h>
#include <stdio.h>

// Test 1: Basic allocation & reset
static bool test_frame_alloc_basic()
{
    size_t         frame_size  = 1024;
    lum_allocator *frame_alloc = lum_create_frame_allocator(frame_size);

    void *p1 = frame_alloc->alloc(frame_alloc, 128, 16);
    void *p2 = frame_alloc->alloc(frame_alloc, 256, 16);

    ASSERT_NOT_NULL(p1);
    ASSERT_NOT_NULL(p2);

    frame_alloc->reset(frame_alloc);

    void *p3 = frame_alloc->alloc(frame_alloc, 512, 16);
    ASSERT_NOT_NULL(p3); // Should work after reset

    lum_destroy_frame_allocator(frame_alloc);
    return true;
}

// Test 2: Allocating beyond frame limits
static bool test_frame_alloc_overflow()
{
    size_t         frame_size  = 512;
    lum_allocator *frame_alloc = lum_create_frame_allocator(frame_size);

    void *p1 = frame_alloc->alloc(frame_alloc, 400, 16);
    ASSERT_NOT_NULL(p1);

    void *p2 = frame_alloc->alloc(frame_alloc, 200, 16); // Should fail
    ASSERT_NULL(p2);

    lum_destroy_frame_allocator(frame_alloc);
    return true;
}

// Test 3: Alignment check
static bool test_frame_alloc_alignment()
{
    size_t         frame_size  = 1024;
    lum_allocator *frame_alloc = lum_create_frame_allocator(frame_size);

    for (size_t i = 16; i <= 64; i *= 2)
    {
        void *p = frame_alloc->alloc(frame_alloc, 128, i);
        ASSERT_NOT_NULL(p);
        ASSERT_TRUE(((uintptr_t) p % i) == 0); // Check alignment
    }

    lum_destroy_frame_allocator(frame_alloc);
    return true;
}

//  Register Tests
TestCase frame_alloc_tests[] = {DECLARE_TEST(test_frame_alloc_basic),
                                DECLARE_TEST(test_frame_alloc_overflow),
                                DECLARE_TEST(test_frame_alloc_alignment)};

int frame_alloc_tests_count = sizeof(frame_alloc_tests) / sizeof(TestCase);
