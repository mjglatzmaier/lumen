#ifndef LUMEN_MATH_VEC4_H
#define LUMEN_MATH_VEC4_H

#include <stdint.h>

typedef struct
{
    float x, y, z, w;
} __attribute__((aligned(16))) lum_vec4; // use alignas(16) c11?

// Adds two vectors
lum_vec4 lum_vec4_add(lum_vec4 a, lum_vec4 b);

// Subtracts two vectors
lum_vec4 lum_vec4_sub(lum_vec4 a, lum_vec4 b);

// Computes the dot product between two vectors
float lum_vec4_dot(lum_vec4 a, lum_vec4 b);

// Scales vector by constant
lum_vec4 lum_vec4_scale(lum_vec4 a, float s);

#endif // LUMEN_MATH_VEC4_H
