#ifndef LUMEN_MATH_VEC3_H
#define LUMEN_MATH_VEC3_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    float x, y, z;
} lum_vec3;

// Adds two vectors
lum_vec3 lum_vec3_add(lum_vec3 a, lum_vec3 b);

// Subtracts two vectors
lum_vec3 lum_vec3_sub(lum_vec3 a, lum_vec3 b);

// Scales vector by constant
float lum_vec3_dot(lum_vec3 a, lum_vec3 b);

// Computes the dot product between two vectors
lum_vec3 lum_vec3_scale(lum_vec3 a, float s);

// Computes the cross-product between two vectors
float lum_vec3_len(lum_vec3 a);

// The length of a vector
float lum_vec3_len2(lum_vec3 a);

// The length squared of a vector
lum_vec3 lum_vec3_normalize(lum_vec3 a);

#endif // LUMEN_MATH_VEC3_H
