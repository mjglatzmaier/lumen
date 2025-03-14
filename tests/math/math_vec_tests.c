#include "../test_framework.h"
#include "math_vec3.h"

#include <math.h>

// **Vec3 Addition**
static bool test_vec3_add(void)
{
    lum_vec3 a      = {1.0f, 2.0f, 3.0f};
    lum_vec3 b      = {4.0f, 5.0f, 6.0f};
    lum_vec3 result = lum_vec3_add(a, b);

    ASSERT_FLOAT_EQ(result.x, 5.0f, 0.0001f);
    ASSERT_FLOAT_EQ(result.y, 7.0f, 0.0001f);
    ASSERT_FLOAT_EQ(result.z, 9.0f, 0.0001f);
    return true;
}

// **Vec3 Dot Product**
static bool test_vec3_dot(void)
{
    lum_vec3 a      = {1.0f, 2.0f, 3.0f};
    lum_vec3 b      = {4.0f, 5.0f, 6.0f};
    float    result = lum_vec3_dot(a, b);
    ASSERT_FLOAT_EQ(result, 32.0f, 0.0001f);
    return true;
}

// **Test Suite**
TestCase vec_tests[] = {
    DECLARE_TEST(test_vec3_add),
    DECLARE_TEST(test_vec3_dot),
};

// **Define the count explicitly**
int vec_tests_count = sizeof(vec_tests) / sizeof(TestCase);