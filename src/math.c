#include "math.h"
#include <math.h>

vec3 vec3_add(vec3 a, vec3 b) {
  return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3 vec3_sub(vec3 a, vec3 b) {
  return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3 vec3_mul(vec3 a, vec3 b) {
  return (vec3){a.x * b.x, a.y * b.y, a.z * b.z};
}

vec3 vec3_div(vec3 a, vec3 b) {
  return (vec3){a.x / b.x, a.y / b.y, a.z / b.z};
}

vec3 vec3_add_scalar(vec3 v, float s) {
  return (vec3){v.x + s, v.y + s, v.z + s};
}

vec3 vec3_sub_scalar(vec3 v, float s) {
  return (vec3){v.x - s, v.y - s, v.z - s};
}

vec3 vec3_mul_scalar(vec3 v, float s) {
  return (vec3){v.x * s, v.y * s, v.z * s};
}

vec3 vec3_div_scalar(vec3 v, float s) {
  return (vec3){v.x / s, v.y / s, v.z / s};
}

float vec3_dot(vec3 a, vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

vec3 vec3_cross(vec3 a, vec3 b) {
  return (vec3){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x};
}

float vec3_length(vec3 v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }

float vec3_length_sq(vec3 v) { return v.x * v.x + v.y * v.y + v.z * v.z; }

vec3 vec3_normalize(vec3 v) {
  float len = vec3_length(v);
  if (len == 0.0f) {
    return (vec3){0.0f, 0.0f, 0.0f};
  }
  return (vec3){v.x / len, v.y / len, v.z / len};
}

vec4 vec4_add(vec4 a, vec4 b) {
  return (vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

vec4 vec4_sub(vec4 a, vec4 b) {
  return (vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

vec4 vec4_mul(vec4 a, vec4 b) {
  return (vec4){a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

vec4 vec4_div(vec4 a, vec4 b) {
  return (vec4){a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
}

vec4 vec4_add_scalar(vec4 v, float s) {
  return (vec4){v.x + s, v.y + s, v.z + s, v.w + s};
}

vec4 vec4_sub_scalar(vec4 v, float s) {
  return (vec4){v.x - s, v.y - s, v.z - s, v.w - s};
}

vec4 vec4_mul_scalar(vec4 v, float s) {
  return (vec4){v.x * s, v.y * s, v.z * s, v.w * s};
}

vec4 vec4_div_scalar(vec4 v, float s) {
  return (vec4){v.x / s, v.y / s, v.z / s, v.w / s};
}

float vec4_dot(vec4 a, vec4 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float vec4_length(vec4 v) {
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

float vec4_length_sq(vec4 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

vec4 vec4_normalize(vec4 v) {
  float len = vec4_length(v);
  if (len == 0.0f) {
    return (vec4){0.0f, 0.0f, 0.0f, 0.0f};
  }
  return (vec4){v.x / len, v.y / len, v.z / len, v.w / len};
}

mat3 mat3_identity() {
  return (mat3){{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
}

mat3 mat3_multiply(mat3 a, mat3 b) {
  mat3 result = {0};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      result.data[i * 3 + j] = a.data[i * 3 + 0] * b.data[0 * 3 + j] +
                               a.data[i * 3 + 1] * b.data[1 * 3 + j] +
                               a.data[i * 3 + 2] * b.data[2 * 3 + j];
    }
  }
  return result;
}

mat3 mat3_scale(float s) {
  return (mat3){{s, 0.0f, 0.0f, 0.0f, s, 0.0f, 0.0f, 0.0f, 1.0f}};
}

mat3 mat3_rotate(mat3 m, float angle) {
  float c = cosf(angle);
  float s = sinf(angle);
  mat3 rotation = {{c, -s, 0.0f, s, c, 0.0f, 0.0f, 0.0f, 1.0f}};
  return mat3_multiply(m, rotation);
}

mat3 mat3_translate(mat3 m, float x, float y) {
  mat3 translation = {{1.0f, 0.0f, x, 0.0f, 1.0f, y, 0.0f, 0.0f, 1.0f}};
  return mat3_multiply(m, translation);
}

mat4 mat4_identity() {
  return (mat4){{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
}

mat4 mat4_multiply(mat4 a, mat4 b) {
  mat4 result = {0};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result.data[i * 4 + j] = a.data[i * 4 + 0] * b.data[0 * 4 + j] +
                               a.data[i * 4 + 1] * b.data[1 * 4 + j] +
                               a.data[i * 4 + 2] * b.data[2 * 4 + j] +
                               a.data[i * 4 + 3] * b.data[3 * 4 + j];
    }
  }
  return result;
}

mat4 mat4_scale(float s) {
  return (mat4){{s, 0.0f, 0.0f, 0.0f, 0.0f, s, 0.0f, 0.0f, 0.0f, 0.0f, s, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f}};
}

mat4 mat4_rotation(vec3 axis, float angle) {
  axis = vec3_normalize(axis);
  float c = cosf(angle);
  float s = sinf(angle);
  float one_minus_c = 1.0f - c;

  float x = axis.x;
  float y = axis.y;
  float z = axis.z;

  return (mat4){{c + x * x * one_minus_c, x * y * one_minus_c - z * s,
                 x * z * one_minus_c + y * s, 0.0f,

                 y * x * one_minus_c + z * s, c + y * y * one_minus_c,
                 y * z * one_minus_c - x * s, 0.0f,

                 z * x * one_minus_c - y * s, z * y * one_minus_c + x * s,
                 c + z * z * one_minus_c, 0.0f,

                 0.0f, 0.0f, 0.0f, 1.0f}};
}

mat4 mat4_rotate(mat4 m, vec3 axis, float angle) {
  axis = vec3_normalize(axis);
  float c = cosf(angle);
  float s = sinf(angle);
  float one_minus_c = 1.0f - c;

  float x = axis.x;
  float y = axis.y;
  float z = axis.z;

  mat4 rotation = {{c + x * x * one_minus_c, x * y * one_minus_c - z * s,
                    x * z * one_minus_c + y * s, 0.0f,

                    y * x * one_minus_c + z * s, c + y * y * one_minus_c,
                    y * z * one_minus_c - x * s, 0.0f,

                    z * x * one_minus_c - y * s, z * y * one_minus_c + x * s,
                    c + z * z * one_minus_c, 0.0f,

                    0.0f, 0.0f, 0.0f, 1.0f}};
  return mat4_multiply(m, rotation);
}

mat4 mat4_translate(mat4 m, vec3 v) {
  mat4 translation = {{1.0f, 0.0f, 0.0f, v.x, 0.0f, 1.0f, 0.0f, v.y, 0.0f, 0.0f,
                       1.0f, v.z, 0.0f, 0.0f, 0.0f, 1.0f}};
  return mat4_multiply(m, translation);
}

rgba_float rgba_div_scalar(rgba rgba, float s) {
  return (rgba_float){rgba.r / s, rgba.g / s, rgba.b / s, rgba.a / s};
}
