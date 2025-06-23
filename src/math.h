#ifndef MATH_H
#define MATH_H

#include <stdint.h>

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

vec3 vec3_scale(vec3 v, float s);
vec3 vec3_translate(vec3 v, vec3 t);

vec4 vec4_scale(vec4 v, float s);
vec4 vec4_translate(vec4 v, vec4 t);

mat3 mat3_identity();
mat3 mat3_multiply(mat3 a, mat3 b);
mat3 mat3_scale(float s);
mat3 mat3_translation(float x, float y);
mat3 mat3_rotation(float angle);

mat4 mat4_identity();
mat4 mat4_multiply(mat4 a, mat4 b);
mat4 mat4_scale(float s);
mat4 mat4_translation(vec3 t);
mat4 mat4_rotation_x(float angle);
mat4 mat4_rotation_y(float angle);
mat4 mat4_rotation_z(float angle);
mat4 mat4_perspective(float fov, float aspect, float near, float far);
mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up);

vec3 vec3_transform_mat3(vec3 v, mat3 m);
vec3 vec3_transform_mat4(vec3 v, mat4 m);
vec4 vec4_transform_mat4(vec4 v, mat4 m);

rgba_float rgba_div_scalar(rgba rgba, float s);

#endif
