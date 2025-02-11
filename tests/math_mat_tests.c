#include "math_mat4.h"
#include "test_framework.h"

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

// **Disabled Test Example**
bool DISABLED_test_mat4_fail() {
    lum_mat4 m;
    lum_mat4_identity(&m);
    ASSERT_FLOAT_EQ(m.m[0], 2.0f, 0.0001f); // This should fail
    return true;
}

// **Test Suite**
TestCase mat_tests[] = {
    DECLARE_TEST(test_mat4_identity),
    DECLARE_TEST(DISABLED_test_mat4_fail),
};

// **Define the count explicitly**
int mat_tests_count = sizeof(mat_tests) / sizeof(TestCase);