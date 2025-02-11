#ifndef LUM_MAT4_H
#define LUM_MAT4_H

#include "math_vec4.h"
#include "math_vec3.h"
#include <stdbool.h>

// 4x4 Matrix (Row-Major)
typedef struct {
    #ifdef USE_SIMD
    float m[16] __attribute__((aligned(16))); // msvc __declspec(align(16))
    #else
    float m[16];
    #endif
} lum_mat4;

// Basic Operations
void lum_mat4_identity(lum_mat4* out);
void lum_mat4_transpose(lum_mat4* out, const lum_mat4* m);
void lum_mat4_scale(lum_mat4* out, const lum_mat4* m, float scalar);
void lum_mat4_add(lum_mat4* out, const lum_mat4* A, const lum_mat4* B);
void lum_mat4_sub(lum_mat4* out, const lum_mat4* A, const lum_mat4* B);
void lum_mat4_mul(lum_mat4* out, const lum_mat4* A, const lum_mat4* B);
void lum_mat4_mul_vec4(lum_vec4* out, const lum_mat4* m, const lum_vec4* v);

// Row/Column Access
lum_vec4 lum_mat4_get_row(const lum_mat4* m, int i);
lum_vec4 lum_mat4_get_col(const lum_mat4* m, int i);
void lum_mat4_set_row(lum_mat4* m, int i, const lum_vec4* row);
void lum_mat4_set_col(lum_mat4* m, int i, const lum_vec4* col);

// Transformations
void lum_mat4_translate(lum_mat4* out, float x, float y, float z);
void lum_mat4_from_rotation_x(lum_mat4* out, float angle);
void lum_mat4_from_rotation_y(lum_mat4* out, float angle);
void lum_mat4_from_rotation_z(lum_mat4* out, float angle);
void lum_mat4_from_axis_angle(lum_mat4* out, const lum_vec3* axis, float angle);

// Projections
void lum_mat4_perspective(lum_mat4* out, float fov, float aspect, float near, float far);
void lum_mat4_orthographic(lum_mat4* out, float left, float right, float bottom, float top, float near, float far);

// Advanced Linear Algebra
float lum_mat4_determinant(const lum_mat4* m);
bool lum_mat4_inverse(lum_mat4* out, const lum_mat4* m);
void lum_mat4_orthogonalize(lum_mat4* out, const lum_mat4* m);

#endif  // LUM_MAT4_H