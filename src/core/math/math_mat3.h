#ifndef LUM_MAT3_H
#define LUM_MAT3_H

#include "math_vec3.h"

typedef struct {
    float m[9];  // Flat 3x3 matrix (row-major order)
} lum_mat3;

void lum_mat3_identity(lum_mat3* out);

// Matrix multiplication: out = A * B
void lum_mat3_mul(lum_mat3* out, const lum_mat3* A, const lum_mat3* B);

// Matrix multiplication with scaling: out = alpha * (A * B) + beta * out
void lum_mat3_mul_add(lum_mat3* out, const lum_mat3* A, const lum_mat3* B, float alpha, float beta);

// Matrix-vector multiplication: out = M * v
void lum_mat3_mul_vec3(lum_vec3* out, const lum_mat3* m, const lum_vec3* v);

// Transpose: out = M^T
void lum_mat3_transpose(lum_mat3* out, const lum_mat3* m);

// Inverse matrix: out = M⁻¹ (returns false if singular)
bool lum_mat3_inverse(lum_mat3* out, const lum_mat3* m);

// Determinant: det(M)
float lum_mat3_determinant(const lum_mat3* m);

// Scale matrix: out = s * M
void lum_mat3_scale(lum_mat3* out, const lum_mat3* m, float scalar);

// Matrix addition: out = A + B
void lum_mat3_add(lum_mat3* out, const lum_mat3* A, const lum_mat3* B);

// Matrix subtraction: out = A - B
void lum_mat3_sub(lum_mat3* out, const lum_mat3* A, const lum_mat3* B);

// Row extraction: out = i-th row of M
lum_vec3 lum_mat3_get_row(const lum_mat3* m, int i);

// Column extraction: out = i-th column of M
lum_vec3 lum_mat3_get_col(const lum_mat3* m, int i);

// Set row i in M
void lum_mat3_set_row(lum_mat3* m, int i, const lum_vec3* row);

// Set column i in M
void lum_mat3_set_col(lum_mat3* m, int i, const lum_vec3* col);

// Create a rotation matrix from axis & angle (column-major order)
void lum_mat3_from_axis_angle(lum_mat3* out, const lum_vec3* axis, float angle);

// Create a 2D rotation matrix (useful for graphics)
void lum_mat3_from_rotation_2d(lum_mat3* out, float angle);

// Orthogonalize a matrix (Gram-Schmidt)
void lum_mat3_orthogonalize(lum_mat3* out, const lum_mat3* m);

// Print the matrix (Debugging)
void lum_mat3_print(const lum_mat3* m);

#endif  // LUM_MAT3_H
