#include "math_vec3.h"
#include <math.h>

// Adds two vectors
lum_vec3 lum_vec3_add(lum_vec3 a, lum_vec3 b) {
    return (lum_vec3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

// Subtracts two vectors
lum_vec3 lum_vec3_sub(lum_vec3 a, lum_vec3 b) {
    return (lum_vec3){ a.x - b.x, a.y - b.y, a.z - b.z };
}

// Scales vector by constant
lum_vec3 lum_vec3_mul(lum_vec3 a, float scalar) {
    return (lum_vec3){ a.x * scalar, a.y * scalar, a.z * scalar };
}

// Computes the dot product between two vectors
float lum_vec3_dot(lum_vec3 a, lum_vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Computes the cross-product between two vectors
lum_vec3 lum_vec3_cross(lum_vec3 a, lum_vec3 b) {
    return (lum_vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// The length of a vector
float lum_vec3_len(lum_vec3 a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

// The length squared of a vector
float lum_vec3_len2(lum_vec3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

// Returns a vector of unit length, or zero if len approximately 0.
lum_vec3 lum_vec3_normalize(lum_vec3 a) {
    float len2 = lum_vec3_len2(a);  // Compute squared length

    if (len2 < 1e-8f) {  // Avoid division by zero for very small vectors
        return (lum_vec3){0.0f, 0.0f, 0.0f};  
    }

    float inv_len = 1.0f / sqrtf(len2);  // Compute 1 / length once
    return (lum_vec3){ a.x * inv_len, a.y * inv_len, a.z * inv_len };
}