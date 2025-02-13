#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

// Test Function Type
//typedef bool (*TestFunc)();

typedef struct {
    const char* name;
    bool (*test_func)(void);
} TestCase;

// **Helper Macros**
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        printf("❌ Assertion failed: %s (Line %d)\n", #condition, __LINE__); \
        return false; \
    }

#define ASSERT_NOT_NULL(ptr)                                               \
    do                                                                     \
    {                                                                      \
        if ((ptr) == NULL)                                                 \
        {                                                                  \
            printf("❌ Assertion failed: %s (Line %d)\n", #ptr, __LINE__); \
            return false;                                                  \
        }                                                                  \
    } while (0)

#define ASSERT_NULL(ptr)                                                   \
    do                                                                     \
    {                                                                      \
        if ((ptr) != NULL)                                                 \
        {                                                                  \
            printf("❌ Assertion failed: %s (Line %d)\n", #ptr, __LINE__); \
            return false;                                                  \
        }                                                                  \
    } while (0)

#define ASSERT_FLOAT_EQ(a, b, epsilon) \
    if (fabs((a) - (b)) > epsilon) { \
        printf("❌ Assertion failed: %f != %f (Line %d)\n", (a), (b), __LINE__); \
        return false; \
    }

// **Benchmark Helper**
#define BENCHMARK(test_func, iterations) \
    do { \
        clock_t start = clock(); \
        for (int i = 0; i < iterations; i++) { test_func(); } \
        clock_t end = clock(); \
        printf("⏱️  %s took %f seconds\n", #test_func, (double)(end - start) / CLOCKS_PER_SEC); \
    } while (0)

// **Test Registration**
#define DECLARE_TEST(test_name) { #test_name, test_name }

// **Helper Function for Time Measurement**
static inline double get_wall_time() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec + (time.tv_usec / 1e6);
}

// **Run Individual Test with Timing & Pass/Fail Handling**
#define RUN_TEST(test, pass_count, fail_count) do { \
    printf("🔍 Running %-25s ", test.name); \
    if (test.test_func == NULL) { \
        printf("🚧 Skipped\n"); \
        continue; \
    } \
    clock_t start_clock = clock(); \
    double start_wall = get_wall_time(); \
    bool success = test.test_func(); \
    clock_t end_clock = clock(); \
    double end_wall = get_wall_time(); \
    double cpu_time = (double)(end_clock - start_clock) / CLOCKS_PER_SEC; \
    double wall_time = end_wall - start_wall; \
    if (success) { \
        pass_count++; \
        printf("✅ Passed  🕒 [CPU: %.6fs | Wall: %.6fs]\n", cpu_time, wall_time); \
    } else { \
        fail_count++; \
        printf("❌ Failed  🕒 [CPU: %.6fs | Wall: %.6fs]\n", cpu_time, wall_time); \
    } \
} while(0)

// **Run Test Suite**
#define RUN_TESTS(suite_name, tests, count) do { \
    printf("\n🚀 Running %s...\n", suite_name); \
    int pass_count = 0, fail_count = 0; \
    for (int i = 0; i < count; i++) { \
        RUN_TEST(tests[i], pass_count, fail_count); \
    } \
    printf("🔹 %s Summary: %d Passed, %d Failed\n", suite_name, pass_count, fail_count); \
} while(0)


#endif // TEST_FRAMEWORK_H
