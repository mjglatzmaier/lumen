#include "math_mat.h"

#ifdef USE_SIMD
    #include <simde/x86/sse.h>
    #include <simde/x86/sse4.1.h>
#endif

#include "math_mat.h"

lum_mat3 lum_mat3_identity() {
    return (lum_mat3){
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
}

lum_mat4 lum_mat4_identity() {
    return (lum_mat4){
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

lum_mat3 lum_mat3_mul(lum_mat3 a, lum_mat3 b) {
    lum_mat3 result;

    #ifdef USE_SIMD
        simde__m128 row0 = simde_mm_loadu_ps(&a.m[0]);
        simde__m128 row1 = simde_mm_loadu_ps(&a.m[3]);
        simde__m128 row2 = simde_mm_loadu_ps(&a.m[6]);

        for (int j = 0; j < 3; j++) {
            simde__m128 col = simde_mm_set_ps(0.0f, b.m[2 * 3 + j], b.m[1 * 3 + j], b.m[0 * 3 + j]);

            simde__m128 mul0 = simde_mm_mul_ps(row0, col);
            simde__m128 mul1 = simde_mm_mul_ps(row1, col);
            simde__m128 mul2 = simde_mm_mul_ps(row2, col);

            simde__m128 sum0 = simde_mm_hadd_ps(mul0, mul0);
            simde__m128 sum1 = simde_mm_hadd_ps(mul1, mul1);
            simde__m128 sum2 = simde_mm_hadd_ps(mul2, mul2);

            sum0 = simde_mm_hadd_ps(sum0, sum0);
            sum1 = simde_mm_hadd_ps(sum1, sum1);
            sum2 = simde_mm_hadd_ps(sum2, sum2);

            result.m[0 * 3 + j] = simde_mm_cvtss_f32(sum0);
            result.m[1 * 3 + j] = simde_mm_cvtss_f32(sum1);
            result.m[2 * 3 + j] = simde_mm_cvtss_f32(sum2);
        }
    #else
        // Unrolled scalar multiplication (standard)
        result.m[0] = a.m[0] * b.m[0] + a.m[1] * b.m[3] + a.m[2] * b.m[6];
        result.m[1] = a.m[0] * b.m[1] + a.m[1] * b.m[4] + a.m[2] * b.m[7];
        result.m[2] = a.m[0] * b.m[2] + a.m[1] * b.m[5] + a.m[2] * b.m[8];
    
        result.m[3] = a.m[3] * b.m[0] + a.m[4] * b.m[3] + a.m[5] * b.m[6];
        result.m[4] = a.m[3] * b.m[1] + a.m[4] * b.m[4] + a.m[5] * b.m[7];
        result.m[5] = a.m[3] * b.m[2] + a.m[4] * b.m[5] + a.m[5] * b.m[8];
    
        result.m[6] = a.m[6] * b.m[0] + a.m[7] * b.m[3] + a.m[8] * b.m[6];
        result.m[7] = a.m[6] * b.m[1] + a.m[7] * b.m[4] + a.m[8] * b.m[7];
        result.m[8] = a.m[6] * b.m[2] + a.m[7] * b.m[5] + a.m[8] * b.m[8];
    #endif

    return result;
}

lum_mat4 lum_mat4_mul(lum_mat4 a, lum_mat4 b) {
    lum_mat4 result;    
    #ifdef USE_SIMD
        for (int i = 0; i < 4; i++) {
            // Load one row of matrix A
            simde__m128 rowA = simde_mm_loadu_ps(&a.m[i * 4]);

            for (int j = 0; j < 4; j++) {
                // Load column j of matrix B as a vector
                simde__m128 colB = simde_mm_set_ps(b.m[12 + j], b.m[8 + j], b.m[4 + j], b.m[j]);

                // Multiply rowA and colB element-wise
                simde__m128 mul = simde_mm_mul_ps(rowA, colB);

                // Horizontally sum the four elements
                simde__m128 sum = simde_mm_hadd_ps(mul, mul);
                sum = simde_mm_hadd_ps(sum, sum);

                // Store result in the output matrix
                result.m[i * 4 + j] = simde_mm_cvtss_f32(sum);
            }
        }
#else
            result.m[0]  = a.m[0] * b.m[0] + a.m[1] * b.m[4] + a.m[2] * b.m[8]  + a.m[3] * b.m[12];
            result.m[1]  = a.m[0] * b.m[1] + a.m[1] * b.m[5] + a.m[2] * b.m[9]  + a.m[3] * b.m[13];
            result.m[2]  = a.m[0] * b.m[2] + a.m[1] * b.m[6] + a.m[2] * b.m[10] + a.m[3] * b.m[14];
            result.m[3]  = a.m[0] * b.m[3] + a.m[1] * b.m[7] + a.m[2] * b.m[11] + a.m[3] * b.m[15];
        
            result.m[4]  = a.m[4] * b.m[0] + a.m[5] * b.m[4] + a.m[6] * b.m[8]  + a.m[7] * b.m[12];
            result.m[5]  = a.m[4] * b.m[1] + a.m[5] * b.m[5] + a.m[6] * b.m[9]  + a.m[7] * b.m[13];
            result.m[6]  = a.m[4] * b.m[2] + a.m[5] * b.m[6] + a.m[6] * b.m[10] + a.m[7] * b.m[14];
            result.m[7]  = a.m[4] * b.m[3] + a.m[5] * b.m[7] + a.m[6] * b.m[11] + a.m[7] * b.m[15];
        
            result.m[8]  = a.m[8] * b.m[0] + a.m[9] * b.m[4] + a.m[10] * b.m[8]  + a.m[11] * b.m[12];
            result.m[9]  = a.m[8] * b.m[1] + a.m[9] * b.m[5] + a.m[10] * b.m[9]  + a.m[11] * b.m[13];
            result.m[10] = a.m[8] * b.m[2] + a.m[9] * b.m[6] + a.m[10] * b.m[10] + a.m[11] * b.m[14];
            result.m[11] = a.m[8] * b.m[3] + a.m[9] * b.m[7] + a.m[10] * b.m[11] + a.m[11] * b.m[15];
        
            result.m[12] = a.m[12] * b.m[0] + a.m[13] * b.m[4] + a.m[14] * b.m[8]  + a.m[15] * b.m[12];
            result.m[13] = a.m[12] * b.m[1] + a.m[13] * b.m[5] + a.m[14] * b.m[9]  + a.m[15] * b.m[13];
            result.m[14] = a.m[12] * b.m[2] + a.m[13] * b.m[6] + a.m[14] * b.m[10] + a.m[15] * b.m[14];
            result.m[15] = a.m[12] * b.m[3] + a.m[13] * b.m[7] + a.m[14] * b.m[11] + a.m[15] * b.m[15];
    #endif
    return result;
}

lum_vec3 lum_mat3_mul_vec3(lum_mat3 m, lum_vec3 v) {
    lum_vec3 result;

    // Scalar fallback (reference implementation)
    result.x = m.m[0] * v.x + m.m[1] * v.y + m.m[2] * v.z;
    result.y = m.m[3] * v.x + m.m[4] * v.y + m.m[5] * v.z;
    result.z = m.m[6] * v.x + m.m[7] * v.y + m.m[8] * v.z;
    return result;
}

lum_vec4 lum_mat4_mul_vec4(lum_mat4 m, lum_vec4 v) {
    lum_vec4 result;

    #ifdef USE_SIMD
        simde__m128 vec = simde_mm_load_ps((float*)&v);

        simde__m128 row0 = simde_mm_load_ps(&m.m[0]);
        simde__m128 row1 = simde_mm_load_ps(&m.m[4]);
        simde__m128 row2 = simde_mm_load_ps(&m.m[8]);
        simde__m128 row3 = simde_mm_load_ps(&m.m[12]);

        simde__m128 mul0 = simde_mm_mul_ps(row0, vec);
        simde__m128 mul1 = simde_mm_mul_ps(row1, vec);
        simde__m128 mul2 = simde_mm_mul_ps(row2, vec);
        simde__m128 mul3 = simde_mm_mul_ps(row3, vec);

        simde__m128 sum0 = simde_mm_hadd_ps(mul0, mul0);
        simde__m128 sum1 = simde_mm_hadd_ps(mul1, mul1);
        simde__m128 sum2 = simde_mm_hadd_ps(mul2, mul2);
        simde__m128 sum3 = simde_mm_hadd_ps(mul3, mul3);

        sum0 = simde_mm_hadd_ps(sum0, sum0);
        sum1 = simde_mm_hadd_ps(sum1, sum1);
        sum2 = simde_mm_hadd_ps(sum2, sum2);
        sum3 = simde_mm_hadd_ps(sum3, sum3);

        result.x = simde_mm_cvtss_f32(sum0);
        result.y = simde_mm_cvtss_f32(sum1);
        result.z = simde_mm_cvtss_f32(sum2);
        result.w = simde_mm_cvtss_f32(sum3);
    #else
        result.x = m.m[0] * v.x + m.m[1] * v.y + m.m[2] * v.z + m.m[3] * v.w;
        result.y = m.m[4] * v.x + m.m[5] * v.y + m.m[6] * v.z + m.m[7] * v.w;
        result.z = m.m[8] * v.x + m.m[9] * v.y + m.m[10] * v.z + m.m[11] * v.w;
        result.w = m.m[12] * v.x + m.m[13] * v.y + m.m[14] * v.z + m.m[15] * v.w;
    #endif

    return result;
}