#include <stdio.h>
#include "munit.h"
#include "math_vec.h"
#include "math_mat.h"
#include "benchmark.h"  // Include benchmarking utilities


#define BENCH_ITERATIONS 1000000

// Test: Vector Addition
static MunitResult test_vec3_add(const MunitParameter params[], void* data) {
    lum_vec3 a = {1.0f, 2.0f, 3.0f};
    lum_vec3 b = {4.0f, 5.0f, 6.0f};
    lum_vec3 result = lum_vec3_add(a, b);

    munit_assert_float(result.x, ==, 5.0f);
    munit_assert_float(result.y, ==, 7.0f);
    munit_assert_float(result.z, ==, 9.0f);

    return MUNIT_OK;
}

// Test: Vector Dot Product
static MunitResult test_vec3_dot(const MunitParameter params[], void* data) {
    lum_vec3 a = {1.0f, 0.0f, 0.0f};
    lum_vec3 b = {0.0f, 1.0f, 0.0f};
    float result = lum_vec3_dot(a, b);

    munit_assert_float(result, ==, 0.0f);  // Perpendicular vectors -> dot product is 0

    return MUNIT_OK;
}

// Test mat3 identity
static MunitResult test_mat3_identity(const MunitParameter params[], void* data) {
    lum_mat3 mat = lum_mat3_identity();
    lum_mat3 expected = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
    munit_assert_memory_equal(sizeof(lum_mat3), &mat, &expected);
    return MUNIT_OK;
}

// Test mat3 multiplication with identity
static MunitResult test_mat3_mul_identity(const MunitParameter params[], void* data) {
    lum_mat3 mat = {
        2.0f, 3.0f, 4.0f,
        1.0f, 5.0f, 6.0f,
        7.0f, 8.0f, 9.0f
    };
    lum_mat3 result = lum_mat3_mul(mat, lum_mat3_identity());
    munit_assert_memory_equal(sizeof(lum_mat3), &result, &mat);
    return MUNIT_OK;
}

// Test mat4 identity
static MunitResult test_mat4_identity(const MunitParameter params[], void* data) {
    lum_mat4 mat = lum_mat4_identity();
    lum_mat4 expected = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    munit_assert_memory_equal(sizeof(lum_mat4), &mat, &expected);
    return MUNIT_OK;
}

// Test mat3 * vec3
static MunitResult test_mat3_mul_vec3(const MunitParameter params[], void* data) {
    lum_mat3 mat = {
        1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f,
        7.0f, 8.0f, 9.0f
    };
    lum_vec3 vec = {1.0f, 2.0f, 3.0f};
    lum_vec3 result = lum_mat3_mul_vec3(mat, vec);
    lum_vec3 expected = {14.0f, 32.0f, 50.0f};

    munit_assert_memory_equal(sizeof(lum_vec3), &result, &expected);
    return MUNIT_OK;
}

// Test mat4 * vec4
static MunitResult test_mat4_mul_vec4(const MunitParameter params[], void* data) {
    lum_mat4 mat = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    };
    lum_vec4 vec = {1.0f, 2.0f, 3.0f, 4.0f};
    lum_vec4 result = lum_mat4_mul_vec4(mat, vec);
    lum_vec4 expected = {30.0f, 70.0f, 110.0f, 150.0f};

    munit_assert_memory_equal(sizeof(lum_vec4), &result, &expected);
    return MUNIT_OK;
}


// Benchmark mat3 * vec3
static MunitResult benchmark_mat3_mul_vec3(const MunitParameter params[], void* data) {
    lum_mat3 mat = {
        1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f,
        7.0f, 8.0f, 9.0f
    };
    lum_vec3 vec = {1.0f, 2.0f, 3.0f};
    lum_vec3 result;

    // Benchmark scalar multiplication
    double t0 = get_time();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        result = lum_mat3_mul_vec3(mat, vec);
    }
    double scalar_time = get_time() - t0;
    //printf("mat3 * vec3 Benchmark: Scalar = %.6f sec (SIMD disabled)\n", scalar_time);
    return MUNIT_OK;
}

// Benchmark mat4 * vec4
static MunitResult benchmark_mat4_mul_vec4(const MunitParameter params[], void* data) {
    lum_mat4 mat = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    };
    lum_vec4 vec = {1.0f, 2.0f, 3.0f, 4.0f};
    lum_vec4 result;

    // Benchmark scalar multiplication
    double t0 = get_time();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        result = lum_mat4_mul_vec4(mat, vec);
    }
    double scalar_time = get_time() - t0;
    //printf("mat4 * vec4 Benchmark: Scalar = %.6f sec\n", scalar_time);
    return MUNIT_OK;
}

// Test Suite
static MunitTest test_suite_tests[] = {
    { "/vec3_add", test_vec3_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/vec3_dot", test_vec3_dot, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    {"/mat3_identity", test_mat3_identity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/mat3_mul_identity", test_mat3_mul_identity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/mat4_identity", test_mat4_identity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/mat3_mul_vec3", test_mat3_mul_vec3, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/mat4_mul_vec4", test_mat4_mul_vec4, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/benchmark_mat3_mul_vec3", benchmark_mat3_mul_vec3, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/benchmark_mat4_mul_vec4", benchmark_mat4_mul_vec4, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

// Main Test Runner
static const MunitSuite test_suite = {
    "/math", test_suite_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* argv[]) {
    return munit_suite_main(&test_suite, NULL, argc, argv);
}
