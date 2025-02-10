#ifndef LUMEN_MATH_VEC_H
#define LUMEN_MATH_VEC_H

#include <stdint.h>

// Vector types
typedef struct { float x, y; } lum_vec2;
typedef struct { float x, y, z; } lum_vec3;
typedef struct { float x, y, z, w; } lum_vec4;

// Load/store functions
lum_vec3 lum_vec3_load(const float* ptr);
void lum_vec3_store(float* ptr, lum_vec3 v);

// Basic math operations
//inline lum_vec2 lum_vec2_add(lum_vec2 a, lum_vec2 b);
lum_vec3 lum_vec3_add(lum_vec3 a, lum_vec3 b);
//inline lum_vec4 lum_vec4_add(lum_vec4 a, lum_vec4 b);

//inline lum_vec2 lum_vec2_sub(lum_vec2 a, lum_vec2 b);
lum_vec3 lum_vec3_sub(lum_vec3 a, lum_vec3 b);
//inline lum_vec4 lum_vec4_sub(lum_vec4 a, lum_vec4 b);

lum_vec3 lum_vec3_cross(lum_vec3 a, lum_vec3 b);
float lum_vec3_dot(lum_vec3 a, lum_vec3 b);
lum_vec3 lum_vec3_normalize(lum_vec3 v);

#endif // LUMEN_MATH_VEC_H
