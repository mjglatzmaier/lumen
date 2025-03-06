#include "../test_framework.h"
#include "math_fast.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// Number of iterations for benchmarking
#define TEST_ITERATIONS 10000000

// Precompute a libdivide structure for large divisors
struct libdivide_u32_t fast_divisor32;
struct libdivide_u64_t fast_divisor64;

// Optimized libdivide fast mod function
static inline uint32_t fast_mod32(uint32_t x, uint32_t C) {
    return x - libdivide_u32_do(x, &fast_divisor32) * C;
}

static inline uint64_t fast_mod64(uint64_t x, uint64_t C) {
    return x - libdivide_u64_do(x, &fast_divisor64) * C;
}

// Benchmark mod(a, p) with a and p near 2^32-1
static bool benchmark_fastmod32() {
    uint32_t sum1 = 0, sum2 = 0;
    uint32_t a, p;

    // Select a divisor near UINT32_MAX
    p = UINT32_MAX - (rand() % 1000);
    fast_divisor32 = libdivide_u32_gen(p);  // Precompute divisor

    clock_t start, end;

    // Benchmarking % operator
    start = clock();
    for (uint32_t i = 0; i < TEST_ITERATIONS; i++) {
        a = UINT32_MAX - (rand() % 1000);  // Large 'a' near UINT32_MAX
        sum1 += a % p;
    }
    end = clock();
    double mod_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Standard %% operator time: %.6f seconds\n", mod_time);

    // Benchmarking libdivide fast mod
    start = clock();
    for (uint32_t i = 0; i < TEST_ITERATIONS; i++) {
        a = UINT32_MAX - (rand() % 1000);
        sum2 += fast_mod32(a, p);
    }
    end = clock();
    double fastmod_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("libdivide fast mod time: %.6f seconds\n", fastmod_time);

    printf("Speedup factor: %.2fx\n", mod_time / fastmod_time);
}

// **Test Suite**
TestCase mod_tests[] = {
    DECLARE_TEST(benchmark_fastmod32)};

// **Define the count explicitly**
int mod_tests_count = sizeof(mod_tests) / sizeof(TestCase);