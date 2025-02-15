#include "math_vec4.h"

// Scalar Implementation
lum_vec4 lum_vec4_add(lum_vec4 a, lum_vec4 b) {
    return (lum_vec4){ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

lum_vec4 lum_vec4_sub(lum_vec4 a, lum_vec4 b) {
    return (lum_vec4){ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

float lum_vec4_dot(lum_vec4 a, lum_vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

lum_vec4 lum_vec4_scale(lum_vec4 a, float s) {
    return (lum_vec4){ a.x * s, a.y * s, a.z * s, a.w * s };
}