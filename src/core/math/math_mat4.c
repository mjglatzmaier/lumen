#include "math_mat4.h"
#include <math.h>
#include <string.h>

#ifdef USE_SIMD
    #include <simde/x86/sse.h>
    #include <simde/x86/avx.h>
#endif

// Identity Matrix
void lum_mat4_identity(lum_mat4* out) {
    *out = (lum_mat4){
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

// Transpose Matrix
void lum_mat4_transpose(lum_mat4* out, const lum_mat4* m) {
    lum_mat4 result = {
        m->m[0], m->m[4], m->m[8],  m->m[12],
        m->m[1], m->m[5], m->m[9],  m->m[13],
        m->m[2], m->m[6], m->m[10], m->m[14],
        m->m[3], m->m[7], m->m[11], m->m[15]
    };
    *out = result;
}

// Matrix Scaling (Fully Unrolled)
void lum_mat4_scale(lum_mat4* out, const lum_mat4* m, float scalar) {
    out->m[0]  = m->m[0]  * scalar;  out->m[1]  = m->m[1]  * scalar;
    out->m[2]  = m->m[2]  * scalar;  out->m[3]  = m->m[3]  * scalar;

    out->m[4]  = m->m[4]  * scalar;  out->m[5]  = m->m[5]  * scalar;
    out->m[6]  = m->m[6]  * scalar;  out->m[7]  = m->m[7]  * scalar;

    out->m[8]  = m->m[8]  * scalar;  out->m[9]  = m->m[9]  * scalar;
    out->m[10] = m->m[10] * scalar;  out->m[11] = m->m[11] * scalar;

    out->m[12] = m->m[12] * scalar;  out->m[13] = m->m[13] * scalar;
    out->m[14] = m->m[14] * scalar;  out->m[15] = m->m[15] * scalar;
}

// Matrix Addition (Fully Unrolled)
void lum_mat4_add(lum_mat4* out, const lum_mat4* A, const lum_mat4* B) {
    out->m[0]  = A->m[0]  + B->m[0];   out->m[1]  = A->m[1]  + B->m[1];
    out->m[2]  = A->m[2]  + B->m[2];   out->m[3]  = A->m[3]  + B->m[3];
    
    out->m[4]  = A->m[4]  + B->m[4];   out->m[5]  = A->m[5]  + B->m[5];
    out->m[6]  = A->m[6]  + B->m[6];   out->m[7]  = A->m[7]  + B->m[7];

    out->m[8]  = A->m[8]  + B->m[8];   out->m[9]  = A->m[9]  + B->m[9];
    out->m[10] = A->m[10] + B->m[10];  out->m[11] = A->m[11] + B->m[11];

    out->m[12] = A->m[12] + B->m[12];  out->m[13] = A->m[13] + B->m[13];
    out->m[14] = A->m[14] + B->m[14];  out->m[15] = A->m[15] + B->m[15];
}

// Matrix Subtraction (Fully Unrolled)
void lum_mat4_sub(lum_mat4* out, const lum_mat4* A, const lum_mat4* B) {
    out->m[0]  = A->m[0]  - B->m[0];   out->m[1]  = A->m[1]  - B->m[1];
    out->m[2]  = A->m[2]  - B->m[2];   out->m[3]  = A->m[3]  - B->m[3];
    
    out->m[4]  = A->m[4]  - B->m[4];   out->m[5]  = A->m[5]  - B->m[5];
    out->m[6]  = A->m[6]  - B->m[6];   out->m[7]  = A->m[7]  - B->m[7];

    out->m[8]  = A->m[8]  - B->m[8];   out->m[9]  = A->m[9]  - B->m[9];
    out->m[10] = A->m[10] - B->m[10];  out->m[11] = A->m[11] - B->m[11];

    out->m[12] = A->m[12] - B->m[12];  out->m[13] = A->m[13] - B->m[13];
    out->m[14] = A->m[14] - B->m[14];  out->m[15] = A->m[15] - B->m[15];
}

// Matrix Multiplication (Fully Unrolled)
void lum_mat4_mul(lum_mat4* out, const lum_mat4* a, const lum_mat4* b) {
    #ifdef USE_SIMD
    // SIMD Optimized Matrix Multiplication
    for (int i = 0; i < 4; i++) {
        simde__m128 row = simde_mm_load_ps(&a->m[i * 4]);
        
        for (int j = 0; j < 4; j++) {
            simde__m128 col = simde_mm_set_ps(
                b->m[3 * 4 + j], b->m[2 * 4 + j], 
                b->m[1 * 4 + j], b->m[0 * 4 + j]
            );
            simde__m128 mul = simde_mm_mul_ps(row, col);
            out->m[i * 4 + j] = simde_mm_cvtss_f32(simde_mm_hadd_ps(mul, mul));
        }
    }
    #else
    // Scalar Unrolled Multiplication (Faster in Debug Mode)
    out->m[0] = a->m[0] * b->m[0] + a->m[1] * b->m[4] + a->m[2] * b->m[8] + a->m[3] * b->m[12];
    out->m[1] = a->m[0] * b->m[1] + a->m[1] * b->m[5] + a->m[2] * b->m[9] + a->m[3] * b->m[13];
    out->m[2] = a->m[0] * b->m[2] + a->m[1] * b->m[6] + a->m[2] * b->m[10] + a->m[3] * b->m[14];
    out->m[3] = a->m[0] * b->m[3] + a->m[1] * b->m[7] + a->m[2] * b->m[11] + a->m[3] * b->m[15];

    out->m[4] = a->m[4] * b->m[0] + a->m[5] * b->m[4] + a->m[6] * b->m[8] + a->m[7] * b->m[12];
    out->m[5] = a->m[4] * b->m[1] + a->m[5] * b->m[5] + a->m[6] * b->m[9] + a->m[7] * b->m[13];
    out->m[6] = a->m[4] * b->m[2] + a->m[5] * b->m[6] + a->m[6] * b->m[10] + a->m[7] * b->m[14];
    out->m[7] = a->m[4] * b->m[3] + a->m[5] * b->m[7] + a->m[6] * b->m[11] + a->m[7] * b->m[15];

    out->m[8] = a->m[8] * b->m[0] + a->m[9] * b->m[4] + a->m[10] * b->m[8] + a->m[11] * b->m[12];
    out->m[9] = a->m[8] * b->m[1] + a->m[9] * b->m[5] + a->m[10] * b->m[9] + a->m[11] * b->m[13];
    out->m[10] = a->m[8] * b->m[2] + a->m[9] * b->m[6] + a->m[10] * b->m[10] + a->m[11] * b->m[14];
    out->m[11] = a->m[8] * b->m[3] + a->m[9] * b->m[7] + a->m[10] * b->m[11] + a->m[11] * b->m[15];

    out->m[12] = a->m[12] * b->m[0] + a->m[13] * b->m[4] + a->m[14] * b->m[8] + a->m[15] * b->m[12];
    out->m[13] = a->m[12] * b->m[1] + a->m[13] * b->m[5] + a->m[14] * b->m[9] + a->m[15] * b->m[13];
    out->m[14] = a->m[12] * b->m[2] + a->m[13] * b->m[6] + a->m[14] * b->m[10] + a->m[15] * b->m[14];
    out->m[15] = a->m[12] * b->m[3] + a->m[13] * b->m[7] + a->m[14] * b->m[11] + a->m[15] * b->m[15];
    #endif
}


// Matrix-Vector Multiplication (Fully Unrolled)
void lum_mat4_mul_vec4(lum_vec4* out, const lum_mat4* m, const lum_vec4* v) {
    out->x = m->m[0] * v->x + m->m[1] * v->y + m->m[2] * v->z + m->m[3] * v->w;
    out->y = m->m[4] * v->x + m->m[5] * v->y + m->m[6] * v->z + m->m[7] * v->w;
    out->z = m->m[8] * v->x + m->m[9] * v->y + m->m[10] * v->z + m->m[11] * v->w;
    out->w = m->m[12] * v->x + m->m[13] * v->y + m->m[14] * v->z + m->m[15] * v->w;
}

// Get Row i
lum_vec4 lum_mat4_get_row(const lum_mat4* m, int i) {
    return (lum_vec4){ m->m[i * 4], m->m[i * 4 + 1], m->m[i * 4 + 2], m->m[i * 4 + 3] };
}

// Get Column i
lum_vec4 lum_mat4_get_col(const lum_mat4* m, int i) {
    return (lum_vec4){ m->m[i], m->m[i + 4], m->m[i + 8], m->m[i + 12] };
}

// Set Row i
void lum_mat4_set_row(lum_mat4* m, int i, const lum_vec4* row) {
    m->m[i * 4] = row->x;
    m->m[i * 4 + 1] = row->y;
    m->m[i * 4 + 2] = row->z;
    m->m[i * 4 + 3] = row->w;
}

// Set Column i
void lum_mat4_set_col(lum_mat4* m, int i, const lum_vec4* col) {
    m->m[i] = col->x;
    m->m[i + 4] = col->y;
    m->m[i + 8] = col->z;
    m->m[i + 12] = col->w;
}

// Translation Matrix
void lum_mat4_translate(lum_mat4* out, float x, float y, float z) {
    *out = (lum_mat4){
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    };
}

// Rotation Matrix (X-Axis)
void lum_mat4_from_rotation_x(lum_mat4* out, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);

    *out = (lum_mat4){
        1, 0,  0, 0,
        0, c, -s, 0,
        0, s,  c, 0,
        0, 0,  0, 1
    };
}

// Rotation Matrix (Y-Axis)
void lum_mat4_from_rotation_y(lum_mat4* out, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);

    *out = (lum_mat4){
        c,  0, s, 0,
        0,  1, 0, 0,
        -s, 0, c, 0,
        0,  0, 0, 1
    };
}

// Rotation Matrix (Z-Axis)
void lum_mat4_from_rotation_z(lum_mat4* out, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);

    *out = (lum_mat4){
        c, -s, 0, 0,
        s,  c, 0, 0,
        0,  0, 1, 0,
        0,  0, 0, 1
    };
}

// Rotation Matrix (Arbitrary Axis)
void lum_mat4_from_axis_angle(lum_mat4* out, const lum_vec3* axis, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    float one_minus_c = 1.0f - c;

    float x = axis->x;
    float y = axis->y;
    float z = axis->z;

    *out = (lum_mat4){
        c + x * x * one_minus_c,     x * y * one_minus_c - z * s, x * z * one_minus_c + y * s, 0,
        y * x * one_minus_c + z * s, c + y * y * one_minus_c,     y * z * one_minus_c - x * s, 0,
        z * x * one_minus_c - y * s, z * y * one_minus_c + x * s, c + z * z * one_minus_c,     0,
        0, 0, 0, 1
    };
}

// Perspective Projection Matrix
void lum_mat4_perspective(lum_mat4* out, float fov, float aspect, float near, float far) {
    float tan_half_fov = tanf(fov * 0.5f);
    float range = near - far;

    *out = (lum_mat4){
        1.0f / (aspect * tan_half_fov), 0,                0,                         0,
        0,                               1.0f / tan_half_fov, 0,                      0,
        0,                               0,                (far + near) / range,      (2 * far * near) / range,
        0,                               0,                -1.0f,                     0
    };
}

// Orthographic Projection Matrix
void lum_mat4_orthographic(lum_mat4* out, float left, float right, float bottom, float top, float near, float far) {
    float width = right - left;
    float height = top - bottom;
    float depth = far - near;

    *out = (lum_mat4){
        2.0f / width,  0,           0,          -(right + left) / width,
        0,            2.0f / height, 0,          -(top + bottom) / height,
        0,            0,           -2.0f / depth, -(far + near) / depth,
        0,            0,            0,          1
    };
}

// Determinant of a 4x4 Matrix
float lum_mat4_determinant(const lum_mat4* m) {
    float a = m->m[0],  b = m->m[1],  c = m->m[2],  d = m->m[3];
    float e = m->m[4],  f = m->m[5],  g = m->m[6],  h = m->m[7];
    float i = m->m[8],  j = m->m[9],  k = m->m[10], l = m->m[11];
    float m4 = m->m[12], n = m->m[13], o = m->m[14], p = m->m[15];

    return a * (f * (k * p - o * l) - j * (g * p - h * o) + n * (g * l - h * k))
         - e * (b * (k * p - o * l) - j * (c * p - d * o) + n * (c * l - d * k))
         + i * (b * (g * p - h * o) - f * (c * p - d * o) + n * (c * h - d * g))
         - m4 * (b * (g * l - h * k) - f * (c * l - d * k) + j * (c * h - d * g));
}

// Inverse of a 4x4 Matrix
bool lum_mat4_inverse(lum_mat4* out, const lum_mat4* m) {
    float inv[16], det;
    float* mat = (float*)m;

    inv[0]  = mat[5]  * mat[10] * mat[15] - mat[5]  * mat[11] * mat[14] - mat[9]  * mat[6]  * mat[15] +
              mat[9]  * mat[7]  * mat[14] + mat[13] * mat[6]  * mat[11] - mat[13] * mat[7]  * mat[10];

    inv[4]  = -mat[4]  * mat[10] * mat[15] + mat[4]  * mat[11] * mat[14] + mat[8]  * mat[6]  * mat[15] -
              mat[8]  * mat[7]  * mat[14] - mat[12] * mat[6]  * mat[11] + mat[12] * mat[7]  * mat[10];

    inv[8]  = mat[4]  * mat[9] * mat[15] - mat[4]  * mat[11] * mat[13] - mat[8]  * mat[5] * mat[15] +
              mat[8]  * mat[7] * mat[13] + mat[12] * mat[5] * mat[11] - mat[12] * mat[7] * mat[9];

    inv[12] = -mat[4]  * mat[9] * mat[14] + mat[4]  * mat[10] * mat[13] + mat[8]  * mat[5] * mat[14] -
              mat[8]  * mat[6] * mat[13] - mat[12] * mat[5] * mat[10] + mat[12] * mat[6] * mat[9];

    det = mat[0] * inv[0] + mat[1] * inv[4] + mat[2] * inv[8] + mat[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0f / det;

    for (int i = 0; i < 16; i++)
        inv[i] *= det;

    memcpy(out, inv, sizeof(lum_mat4));
    return true;
}

// Orthogonalize a 4x4 Matrix
void lum_mat4_orthogonalize(lum_mat4* out, const lum_mat4* m) {
    lum_vec3 x = { m->m[0], m->m[1], m->m[2] };
    lum_vec3 y = { m->m[4], m->m[5], m->m[6] };
    lum_vec3 z = { m->m[8], m->m[9], m->m[10] };

    // Normalize first column (X-axis)
    float len_x = sqrtf(x.x * x.x + x.y * x.y + x.z * x.z);
    float ilen_x = 1.0f / len_x;
    x.x *= ilen_x; x.y *= ilen_x; x.z *= ilen_x;

    // Make Y-axis orthogonal to X
    float dot_xy = x.x * y.x + x.y * y.y + x.z * y.z;
    y.x -= dot_xy * x.x;
    y.y -= dot_xy * x.y;
    y.z -= dot_xy * x.z;

    // Normalize Y-axis
    float len_y = sqrtf(y.x * y.x + y.y * y.y + y.z * y.z);
    float ilen_y = 1.0f / len_y;
    y.x *= ilen_y; y.y *= ilen_y; y.z *= ilen_y;

    // Make Z-axis orthogonal to X and Y
    float dot_xz = x.x * z.x + x.y * z.y + x.z * z.z;
    float dot_yz = y.x * z.x + y.y * z.y + y.z * z.z;

    z.x -= dot_xz * x.x + dot_yz * y.x;
    z.y -= dot_xz * x.y + dot_yz * y.y;
    z.z -= dot_xz * x.z + dot_yz * y.z;

    // Normalize Z-axis
    float len_z = sqrtf(z.x * z.x + z.y * z.y + z.z * z.z);
    float ilen_z = 1.0f / len_z;
    z.x *= ilen_z; z.y *= ilen_z; z.z *= ilen_z;

    // Rebuild orthogonalized matrix
    *out = (lum_mat4){
        x.x, x.y, x.z, m->m[3],
        y.x, y.y, y.z, m->m[7],
        z.x, z.y, z.z, m->m[11],
        m->m[12], m->m[13], m->m[14], m->m[15]
    };
}