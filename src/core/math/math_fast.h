#ifndef LUM_MATH_FAST_H
#define LUM_MATH_FAST_H

#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

// Fast Trigonometry (sinf, cosf, tanf)
float lum_fast_sinf(float x);
float lum_fast_cosf(float x);
float lum_fast_tanf(float x);

// Fast Inverse Trigonometry (atan2)
float lum_fast_atan2f(float y, float x);

// Fast Exponentials and Logarithms
float lum_fast_expf(float x);
float lum_fast_logf(float x);

// Fast Square Root and Inverse Square Root
float lum_fast_sqrtf(float x);
float lum_fast_rsqrtf(float x);

// Fast Reciprocal (1/x)
float lum_fast_recipf(float x);

// fast mod
int lum_fast_mod_pow2(int x, int N);
 uint32_t lum_fast_mod32(uint32_t word, uint32_t p);
// uint64_t lum_fast_mod64(uint64_t word, uint64_t p);
// int lum_fast_mod(int word, int p);

#ifdef __cplusplus
}
#endif

#endif // LUM_MATH_FAST_H
