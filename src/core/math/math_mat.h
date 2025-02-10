#ifndef LUMEN_MATH_MAT_H
#define LUMEN_MATH_MAT_H

#include <stdint.h>
#include "math_vec.h"


typedef struct {
    float m[9];  // 3x3 matrix stored as a flat array
} lum_mat3;

typedef struct {
    float m[16]; // 4x4 matrix stored as a flat array
} lum_mat4;


lum_mat3 lum_mat3_identity();
lum_mat3 lum_mat3_mul(lum_mat3 a, lum_mat3 b);

lum_mat4 lum_mat4_identity();
lum_mat4 lum_mat4_mul(lum_mat4 a, lum_mat4 b);

// Mat-vec operations
lum_vec3 lum_mat3_mul_vec3(lum_mat3 m, lum_vec3 v);
lum_vec4 lum_mat4_mul_vec4(lum_mat4 m, lum_vec4 v);

#endif // LUMEN_MATH_MAT_H
