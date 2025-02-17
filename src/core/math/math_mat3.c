#include "math_mat3.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h> // For memcpy

// Identity Matrix
void lum_mat3_identity(lum_mat3 *out)
{
    *out = (lum_mat3) {1, 0, 0, 0, 1, 0, 0, 0, 1};
}

// Matrix Multiplication: out = A * B
void lum_mat3_mul(lum_mat3 *out, const lum_mat3 *A, const lum_mat3 *B)
{
    lum_mat3 result;

    // Row 0
    result.m[0] = A->m[0] * B->m[0] + A->m[1] * B->m[3] + A->m[2] * B->m[6];
    result.m[1] = A->m[0] * B->m[1] + A->m[1] * B->m[4] + A->m[2] * B->m[7];
    result.m[2] = A->m[0] * B->m[2] + A->m[1] * B->m[5] + A->m[2] * B->m[8];

    // Row 1
    result.m[3] = A->m[3] * B->m[0] + A->m[4] * B->m[3] + A->m[5] * B->m[6];
    result.m[4] = A->m[3] * B->m[1] + A->m[4] * B->m[4] + A->m[5] * B->m[7];
    result.m[5] = A->m[3] * B->m[2] + A->m[4] * B->m[5] + A->m[5] * B->m[8];

    // Row 2
    result.m[6] = A->m[6] * B->m[0] + A->m[7] * B->m[3] + A->m[8] * B->m[6];
    result.m[7] = A->m[6] * B->m[1] + A->m[7] * B->m[4] + A->m[8] * B->m[7];
    result.m[8] = A->m[6] * B->m[2] + A->m[7] * B->m[5] + A->m[8] * B->m[8];

    *out = result;
}

// Matrix-Vector Multiplication: out = M * v
void lum_mat3_mul_vec3(lum_vec3 *out, const lum_mat3 *m, const lum_vec3 *v)
{
    out->x = m->m[0] * v->x + m->m[1] * v->y + m->m[2] * v->z;
    out->y = m->m[3] * v->x + m->m[4] * v->y + m->m[5] * v->z;
    out->z = m->m[6] * v->x + m->m[7] * v->y + m->m[8] * v->z;
}

// Transpose Matrix: out = M^T
void lum_mat3_transpose(lum_mat3 *out, const lum_mat3 *m)
{
    lum_mat3 result;
    result.m[0] = m->m[0];
    result.m[1] = m->m[3];
    result.m[2] = m->m[6];
    result.m[3] = m->m[1];
    result.m[4] = m->m[4];
    result.m[5] = m->m[7];
    result.m[6] = m->m[2];
    result.m[7] = m->m[5];
    result.m[8] = m->m[8];
    *out        = result;
}

// Determinant of Matrix
float lum_mat3_determinant(const lum_mat3 *m)
{
    return m->m[0] * (m->m[4] * m->m[8] - m->m[5] * m->m[7]) -
           m->m[1] * (m->m[3] * m->m[8] - m->m[5] * m->m[6]) +
           m->m[2] * (m->m[3] * m->m[7] - m->m[4] * m->m[6]);
}

// Inverse Matrix (returns false if singular)
bool lum_mat3_inverse(lum_mat3 *out, const lum_mat3 *m)
{
    float det = lum_mat3_determinant(m);
    if (fabs(det) < 1e-6f)
        return false; // Singular matrix, no inverse

    float    inv_det = 1.0f / det;
    lum_mat3 result;

    result.m[0] = (m->m[4] * m->m[8] - m->m[5] * m->m[7]) * inv_det;
    result.m[1] = (m->m[2] * m->m[7] - m->m[1] * m->m[8]) * inv_det;
    result.m[2] = (m->m[1] * m->m[5] - m->m[2] * m->m[4]) * inv_det;

    result.m[3] = (m->m[5] * m->m[6] - m->m[3] * m->m[8]) * inv_det;
    result.m[4] = (m->m[0] * m->m[8] - m->m[2] * m->m[6]) * inv_det;
    result.m[5] = (m->m[2] * m->m[3] - m->m[0] * m->m[5]) * inv_det;

    result.m[6] = (m->m[3] * m->m[7] - m->m[4] * m->m[6]) * inv_det;
    result.m[7] = (m->m[1] * m->m[6] - m->m[0] * m->m[7]) * inv_det;
    result.m[8] = (m->m[0] * m->m[4] - m->m[1] * m->m[3]) * inv_det;

    *out = result;
    return true;
}

// Scale Matrix: out = s * M
void lum_mat3_scale(lum_mat3 *out, const lum_mat3 *m, float scalar)
{
    out->m[0] = m->m[0] * scalar;
    out->m[1] = m->m[1] * scalar;
    out->m[2] = m->m[2] * scalar;
    out->m[3] = m->m[3] * scalar;
    out->m[4] = m->m[4] * scalar;
    out->m[5] = m->m[5] * scalar;
    out->m[6] = m->m[6] * scalar;
    out->m[7] = m->m[7] * scalar;
    out->m[8] = m->m[8] * scalar;
}

// Matrix Addition: out = A + B
void lum_mat3_add(lum_mat3 *out, const lum_mat3 *A, const lum_mat3 *B)
{
    out->m[0] = A->m[0] + B->m[0];
    out->m[1] = A->m[1] + B->m[1];
    out->m[2] = A->m[2] + B->m[2];
    out->m[3] = A->m[3] + B->m[3];
    out->m[4] = A->m[4] + B->m[4];
    out->m[5] = A->m[5] + B->m[5];
    out->m[6] = A->m[6] + B->m[6];
    out->m[7] = A->m[7] + B->m[7];
    out->m[8] = A->m[8] + B->m[8];
}

// Matrix Subtraction: out = A - B
void lum_mat3_sub(lum_mat3 *out, const lum_mat3 *A, const lum_mat3 *B)
{
    out->m[0] = A->m[0] - B->m[0];
    out->m[1] = A->m[1] - B->m[1];
    out->m[2] = A->m[2] - B->m[2];
    out->m[3] = A->m[3] - B->m[3];
    out->m[4] = A->m[4] - B->m[4];
    out->m[5] = A->m[5] - B->m[5];
    out->m[6] = A->m[6] - B->m[6];
    out->m[7] = A->m[7] - B->m[7];
    out->m[8] = A->m[8] - B->m[8];
}

// Get Row i (returns as a `vec3`)
lum_vec3 lum_mat3_get_row(const lum_mat3 *m, int i)
{
    return (lum_vec3) {m->m[i * 3], m->m[i * 3 + 1], m->m[i * 3 + 2]};
}

// Get Column i (returns as a `vec3`)
lum_vec3 lum_mat3_get_col(const lum_mat3 *m, int i)
{
    return (lum_vec3) {m->m[i], m->m[i + 3], m->m[i + 6]};
}

// Set Row i in M
void lum_mat3_set_row(lum_mat3 *m, int i, const lum_vec3 *row)
{
    m->m[i * 3]     = row->x;
    m->m[i * 3 + 1] = row->y;
    m->m[i * 3 + 2] = row->z;
}

// Set Column i in M
void lum_mat3_set_col(lum_mat3 *m, int i, const lum_vec3 *col)
{
    m->m[i]     = col->x;
    m->m[i + 3] = col->y;
    m->m[i + 6] = col->z;
}

// Create Rotation Matrix from Axis & Angle (Rodrigues' Formula)
void lum_mat3_from_axis_angle(lum_mat3 *out, const lum_vec3 *axis, float angle)
{
    float c           = cosf(angle);
    float s           = sinf(angle);
    float one_minus_c = 1.0f - c;

    lum_vec3 a = *axis;

    out->m[0] = c + a.x * a.x * one_minus_c;
    out->m[1] = a.x * a.y * one_minus_c - a.z * s;
    out->m[2] = a.x * a.z * one_minus_c + a.y * s;

    out->m[3] = a.y * a.x * one_minus_c + a.z * s;
    out->m[4] = c + a.y * a.y * one_minus_c;
    out->m[5] = a.y * a.z * one_minus_c - a.x * s;

    out->m[6] = a.z * a.x * one_minus_c - a.y * s;
    out->m[7] = a.z * a.y * one_minus_c + a.x * s;
    out->m[8] = c + a.z * a.z * one_minus_c;
}

// Create 2D Rotation Matrix
void lum_mat3_from_rotation_2d(lum_mat3 *out, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);

    *out = (lum_mat3) {c, -s, 0, s, c, 0, 0, 0, 1};
}

// Orthogonalize a Matrix (Modified Gram-Schmidt)
void lum_mat3_orthogonalize(lum_mat3 *out, const lum_mat3 *m)
{
    lum_vec3 col0 = lum_mat3_get_col(m, 0);
    lum_vec3 col1 = lum_mat3_get_col(m, 1);
    lum_vec3 col2 = lum_mat3_get_col(m, 2);

    // Gram-Schmidt orthogonalization
    col0 = lum_vec3_normalize(col0);
    col1 = lum_vec3_sub(col1, lum_vec3_scale(col0, lum_vec3_dot(col1, col0)));
    col1 = lum_vec3_normalize(col1);
    col2 = lum_vec3_sub(col2, lum_vec3_scale(col0, lum_vec3_dot(col2, col0)));
    col2 = lum_vec3_sub(col2, lum_vec3_scale(col1, lum_vec3_dot(col2, col1)));
    col2 = lum_vec3_normalize(col2);

    lum_mat3_set_col(out, 0, &col0);
    lum_mat3_set_col(out, 1, &col1);
    lum_mat3_set_col(out, 2, &col2);
}

// Print Matrix (for Debugging)
void lum_mat3_print(const lum_mat3 *m)
{
    printf("[ %.3f %.3f %.3f ]\n", m->m[0], m->m[1], m->m[2]);
    printf("[ %.3f %.3f %.3f ]\n", m->m[3], m->m[4], m->m[5]);
    printf("[ %.3f %.3f %.3f ]\n", m->m[6], m->m[7], m->m[8]);
}
