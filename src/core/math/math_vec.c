#include "math_vec.h"
#include <math.h>

#ifdef USE_SIMD
    #include <simde/x86/sse.h>  // Portable SIMD implementation
#endif

// Load/store functions (for SIMD)
lum_vec3 lum_vec3_load(const float* ptr) {
    lum_vec3 v;
    #ifdef USE_SIMD
        simde__m128 simd_v = simde_mm_loadu_ps(ptr);
        simde_mm_storeu_ps(&v.x, simd_v);
    #else
        v.x = ptr[0]; v.y = ptr[1]; v.z = ptr[2];
    #endif
    return v;
}

void lum_vec3_store(float* ptr, lum_vec3 v) {
    #ifdef USE_SIMD
        simde__m128 simd_v = simde_mm_set_ps(0, v.z, v.y, v.x);
        simde_mm_storeu_ps(ptr, simd_v);
    #else
        ptr[0] = v.x; ptr[1] = v.y; ptr[2] = v.z;
    #endif
}

// Vector addition
lum_vec3 lum_vec3_add(lum_vec3 a, lum_vec3 b) {
    #ifdef USE_SIMD
        simde__m128 va = simde_mm_set_ps(0, a.z, a.y, a.x);
        simde__m128 vb = simde_mm_set_ps(0, b.z, b.y, b.x);
        simde__m128 result = simde_mm_add_ps(va, vb);
        lum_vec3 v;
        simde_mm_storeu_ps(&v.x, result);
        return v;
    #else
        return (lum_vec3){ a.x + b.x, a.y + b.y, a.z + b.z };
    #endif
}

// Vector subtraction
lum_vec3 lum_vec3_sub(lum_vec3 a, lum_vec3 b) {
    #ifdef USE_SIMD
        simde__m128 va = simde_mm_set_ps(0, a.z, a.y, a.x);
        simde__m128 vb = simde_mm_set_ps(0, b.z, b.y, b.x);
        simde__m128 result = simde_mm_sub_ps(va, vb);
        lum_vec3 v;
        simde_mm_storeu_ps(&v.x, result);
        return v;
    #else
        return (lum_vec3){ a.x - b.x, a.y - b.y, a.z - b.z };
    #endif
}

// Vector cross product
lum_vec3 lum_vec3_cross(lum_vec3 a, lum_vec3 b) {
    return (lum_vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// Vector dot product
float lum_vec3_dot(lum_vec3 a, lum_vec3 b) {
    #ifdef USE_SIMD
        simde__m128 va = simde_mm_set_ps(0, a.z, a.y, a.x);
        simde__m128 vb = simde_mm_set_ps(0, b.z, b.y, b.x);
        simde__m128 mul = simde_mm_mul_ps(va, vb);
        float result[4];
        simde_mm_storeu_ps(result, mul);
        return result[0] + result[1] + result[2];
    #else
        return a.x * b.x + a.y * b.y + a.z * b.z;
    #endif
}

// Vector normalization
lum_vec3 lum_vec3_normalize(lum_vec3 v) {
    float length = lum_vec3_dot(v, v);
    if (length > 0) {
        float inv_length = 1.0f / sqrtf(length);
        return (lum_vec3){ v.x * inv_length, v.y * inv_length, v.z * inv_length };
    }
    return v;
}
