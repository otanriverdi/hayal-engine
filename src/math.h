#ifndef MATH_H
#define MATH_H

#include <stdint.h>

typedef struct vec2 {
  float x, y;
} vec2;

typedef struct vec3 {
  float x, y, z;
} vec3;

typedef struct vec4 {
  float x, y, z, w;
} vec4;

typedef struct rgba {
  uint8_t r, g, b, a;
} rgba;

typedef struct rgba_float {
  float r, g, b, a;
} rgba_float;

typedef struct mat3 {
  float data[9];
} mat3;

typedef struct mat4 {
  float data[16];
} mat4;

vec2 vec2_add(vec2 a, vec2 b);
vec2 vec2_sub(vec2 a, vec2 b);
vec2 vec2_mul(vec2 a, vec2 b);
vec2 vec2_div(vec2 a, vec2 b);

vec2 vec2_add_scalar(vec2 v, float s);
vec2 vec2_sub_scalar(vec2 v, float s);
vec2 vec2_mul_scalar(vec2 v, float s);
vec2 vec2_div_scalar(vec2 v, float s);

float vec2_dot(vec2 a, vec2 b);
float vec2_length(vec2 v);
float vec2_length_sq(vec2 v);
vec2 vec2_normalize(vec2 v);

vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_mul(vec3 a, vec3 b);
vec3 vec3_div(vec3 a, vec3 b);

vec3 vec3_add_scalar(vec3 v, float s);
vec3 vec3_sub_scalar(vec3 v, float s);
vec3 vec3_mul_scalar(vec3 v, float s);
vec3 vec3_div_scalar(vec3 v, float s);

float vec3_dot(vec3 a, vec3 b);
vec3 vec3_cross(vec3 a, vec3 b);
float vec3_length(vec3 v);
float vec3_length_sq(vec3 v);
vec3 vec3_normalize(vec3 v);

vec4 vec4_add(vec4 a, vec4 b);
vec4 vec4_sub(vec4 a, vec4 b);
vec4 vec4_mul(vec4 a, vec4 b);
vec4 vec4_div(vec4 a, vec4 b);

vec4 vec4_add_scalar(vec4 v, float s);
vec4 vec4_sub_scalar(vec4 v, float s);
vec4 vec4_mul_scalar(vec4 v, float s);
vec4 vec4_div_scalar(vec4 v, float s);

float vec4_dot(vec4 a, vec4 b);
float vec4_length(vec4 v);
float vec4_length_sq(vec4 v);
vec4 vec4_normalize(vec4 v);

mat4 mat4_identity();
mat4 mat4_multiply(mat4 a, mat4 b);
mat4 mat4_scale(mat4 m, vec3 s);
mat4 mat4_rotate(mat4 m, vec3 axis, float angle);
mat4 mat4_translate(mat4 m, vec3 v);
mat4 mat4_perspective(float fov, float aspect, float near, float far);
mat4 mat4_ortho(float left, float right, float bottom, float top, float near, float far);

rgba_float rgba_div_scalar(rgba rgba, float s);

float to_radian(float degrees);

#endif
