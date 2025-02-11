#include "math_fast.h"

// Fast Approximate `sinf(x)` (Remez Polynomial Approximation)
// From quake
float lum_fast_sinf(float x) {
    const float B = 4.0f / M_PI;
    const float C = -4.0f / (M_PI * M_PI);
    const float P = 0.225f;

    float y = B * x + C * x * fabsf(x);
    return P * (y * fabsf(y) - y) + y;
}

// Fast Approximate `cosf(x)` (Derived from sinf)
float lum_fast_cosf(float x) {
    return lum_fast_sinf(x + M_PI_2);
}

// Fast Approximate `tanf(x)` (Using sin/cos)
float lum_fast_tanf(float x) {
    return lum_fast_sinf(x) / lum_fast_cosf(x);
}

// Fast `atan2(y, x)` Approximation (Chris Lomont's Method)
float lum_fast_atan2f(float y, float x) {
    const float ONEQTR_PI = M_PI / 4.0f;
    const float THRQTR_PI = 3.0f * M_PI / 4.0f;
    float abs_y = fabsf(y) + 1e-10f;  // Prevent division by zero
    float angle;
    
    if (x >= 0) {
        float r = (x - abs_y) / (x + abs_y);
        angle = ONEQTR_PI - ONEQTR_PI * r;
    } else {
        float r = (x + abs_y) / (abs_y - x);
        angle = THRQTR_PI - ONEQTR_PI * r;
    }
    return (y < 0) ? -angle : angle;
}

// Fast `expf(x)` (2^x Approximation)
float lum_fast_expf(float x) {
    union { float f; int32_t i; } u;
    u.i = (int32_t)(12102203.0f * x) + 127 * (1 << 23);
    return u.f;
}

// Fast `logf(x)` Approximation
float lum_fast_logf(float x) {
    union { float f; int32_t i; } u = { x };
    return (u.i - 1064866805) * 8.262958405176314e-8f;
}

// Fast `sqrtf(x)` using Newton-Raphson
float lum_fast_sqrtf(float x) {
    return x * lum_fast_rsqrtf(x);
}

// Fast `rsqrtf(x)` using Quake III's Method
float lum_fast_rsqrtf(float x) {
    float xhalf = 0.5f * x;
    union { float f; int32_t i; } u = { x };
    u.i = 0x5f3759df - (u.i >> 1);
    x = u.f;
    return x * (1.5f - xhalf * x * x);
}

// Fast Reciprocal (1/x) using bit manipulation
float lum_fast_recipf(float x) {
    union { float f; int32_t i; } u = { x };
    u.i = 0x7EEEEEEE - u.i;
    return u.f;
}
