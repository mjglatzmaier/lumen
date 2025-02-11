#include "math_mat4.h"
#include "test_framework.h"

// Number of iterations for benchmarking
#define BENCH_ITERATIONS 1000000

// **Mat4 Identity Test**
bool test_mat4_identity() {
    lum_mat4 m;
    lum_mat4_identity(&m);
    ASSERT_FLOAT_EQ(m.m[0], 1.0f, 0.0001f);
    ASSERT_FLOAT_EQ(m.m[5], 1.0f, 0.0001f);
    ASSERT_FLOAT_EQ(m.m[10], 1.0f, 0.0001f);
    ASSERT_FLOAT_EQ(m.m[15], 1.0f, 0.0001f);
    return true;
}

static bool test_mat4_mul_bench() {
    lum_mat4 a, b, result;
    lum_mat4_identity(&a);
    lum_mat4_identity(&b);
    // Fill
    for (int i = 0; i < 16; i++) {
        a.m[i] = (float)(i + 1);
        b.m[i] = (float)(16 - i);
    }
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        lum_mat4_mul(&result, &a, &b);
    }
    return true;
}

static bool test_mat4_scale_bench() {
    lum_mat4 a, result;
    lum_mat4_identity(&a);
    // Fill
    for (int i = 0; i < 16; i++) {
        a.m[i] = (float)(i + 1);
    }
    float scalar = 3.14159f;
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        lum_mat4_scale(&result, &a, scalar);
    }
    return true;
}

static bool test_mat4_transpose_bench() {
    lum_mat4 a, result;
    lum_mat4_identity(&a);
    // Fill
    for (int i = 0; i < 16; i++) {
        a.m[i] = (float)(i + 1);
    }
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        lum_mat4_transpose(&result, &a);
    }
    return true;
}

static bool test_mat4_add_bench() {
    lum_mat4 a, b, result;
    lum_mat4_identity(&a);
    lum_mat4_identity(&b);
    // Fill
    for (int i = 0; i < 16; i++) {
        a.m[i] = (float)(i + 1);
        b.m[i] = (float)(16 - i);
    }
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        lum_mat4_add(&result, &a, &b);
    }
    return true;
}

static bool test_mat4_vec4_bench() {
    lum_mat4 a;
    lum_vec4 b, result;
    lum_mat4_identity(&a);
    // Fill
    for (int i = 0; i < 16; i++) {
        a.m[i] = (float)(i + 1);
    }
    b = (lum_vec4){ 5.0, 4.0, 3.0, 2.0 };

    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        lum_mat4_mul_vec4(&result, &a, &b);
    }
    return true;
}

// **Test Suite**
TestCase mat_tests[] = {
    DECLARE_TEST(test_mat4_identity),
    DECLARE_TEST(test_mat4_mul_bench),
    DECLARE_TEST(test_mat4_scale_bench),
    DECLARE_TEST(test_mat4_transpose_bench),
    DECLARE_TEST(test_mat4_vec4_bench)
};

// **Define the count explicitly**
int mat_tests_count = sizeof(mat_tests) / sizeof(TestCase);