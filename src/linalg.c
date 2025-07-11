#include "linalg.h"
#include <math.h>

vec2 vec2_add(vec2 a, vec2 b) { return (vec2){a.x + b.x, a.y + b.y}; }

vec2 vec2_sub(vec2 a, vec2 b) { return (vec2){a.x - b.x, a.y - b.y}; }

vec2 vec2_mul(vec2 a, vec2 b) { return (vec2){a.x * b.x, a.y * b.y}; }

vec2 vec2_div(vec2 a, vec2 b) { return (vec2){a.x / b.x, a.y / b.y}; }

vec2 vec2_add_scalar(vec2 v, float s) { return (vec2){v.x + s, v.y + s}; }

vec2 vec2_sub_scalar(vec2 v, float s) { return (vec2){v.x - s, v.y - s}; }

vec2 vec2_mul_scalar(vec2 v, float s) { return (vec2){v.x * s, v.y * s}; }

vec2 vec2_div_scalar(vec2 v, float s) { return (vec2){v.x / s, v.y / s}; }

float vec2_dot(vec2 a, vec2 b) { return a.x * b.x + a.y * b.y; }

float vec2_length(vec2 v) { return sqrtf(v.x * v.x + v.y * v.y); }

float vec2_length_sq(vec2 v) { return v.x * v.x + v.y * v.y; }

vec2 vec2_normalize(vec2 v) {
  float len = vec2_length(v);
  if (len == 0.0f) {
    return (vec2){0.0f, 0.0f};
  }
  return (vec2){v.x / len, v.y / len};
}

vec3 vec3_add(vec3 a, vec3 b) { return (vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }

vec3 vec3_sub(vec3 a, vec3 b) { return (vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }

vec3 vec3_mul(vec3 a, vec3 b) { return (vec3){a.x * b.x, a.y * b.y, a.z * b.z}; }

vec3 vec3_div(vec3 a, vec3 b) { return (vec3){a.x / b.x, a.y / b.y, a.z / b.z}; }

vec3 vec3_add_scalar(vec3 v, float s) { return (vec3){v.x + s, v.y + s, v.z + s}; }

vec3 vec3_sub_scalar(vec3 v, float s) { return (vec3){v.x - s, v.y - s, v.z - s}; }

vec3 vec3_mul_scalar(vec3 v, float s) { return (vec3){v.x * s, v.y * s, v.z * s}; }

vec3 vec3_div_scalar(vec3 v, float s) { return (vec3){v.x / s, v.y / s, v.z / s}; }

float vec3_dot(vec3 a, vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

vec3 vec3_cross(vec3 a, vec3 b) {
  return (vec3){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
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

vec4 vec4_add(vec4 a, vec4 b) { return (vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }

vec4 vec4_sub(vec4 a, vec4 b) { return (vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; }

vec4 vec4_mul(vec4 a, vec4 b) { return (vec4){a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}; }

vec4 vec4_div(vec4 a, vec4 b) { return (vec4){a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w}; }

vec4 vec4_add_scalar(vec4 v, float s) { return (vec4){v.x + s, v.y + s, v.z + s, v.w + s}; }

vec4 vec4_sub_scalar(vec4 v, float s) { return (vec4){v.x - s, v.y - s, v.z - s, v.w - s}; }

vec4 vec4_mul_scalar(vec4 v, float s) { return (vec4){v.x * s, v.y * s, v.z * s, v.w * s}; }

vec4 vec4_div_scalar(vec4 v, float s) { return (vec4){v.x / s, v.y / s, v.z / s, v.w / s}; }

float vec4_dot(vec4 a, vec4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

float vec4_length(vec4 v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w); }

float vec4_length_sq(vec4 v) { return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w; }

vec4 vec4_normalize(vec4 v) {
  float len = vec4_length(v);
  if (len == 0.0f) {
    return (vec4){0.0f, 0.0f, 0.0f, 0.0f};
  }
  return (vec4){v.x / len, v.y / len, v.z / len, v.w / len};
}

mat4 mat4_identity() {
  return (mat4){
      {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
}

mat4 mat4_multiply(mat4 a, mat4 b) {
  mat4 result = {0};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result.data[j * 4 + i] = a.data[0 * 4 + i] * b.data[j * 4 + 0] + a.data[1 * 4 + i] * b.data[j * 4 + 1] +
                               a.data[2 * 4 + i] * b.data[j * 4 + 2] + a.data[3 * 4 + i] * b.data[j * 4 + 3];
    }
  }
  return result;
}

mat4 mat4_scale(mat4 m, vec3 s) {
  m.data[0] *= s.x;
  m.data[1] *= s.x;
  m.data[2] *= s.x;
  m.data[3] *= s.x;

  m.data[4] *= s.y;
  m.data[5] *= s.y;
  m.data[6] *= s.y;
  m.data[7] *= s.y;

  m.data[8] *= s.z;
  m.data[9] *= s.z;
  m.data[10] *= s.z;
  m.data[11] *= s.z;

  return m;
}

mat4 mat4_rotation(vec3 axis, float angle) {
  axis = vec3_normalize(axis);
  float c = cosf(angle);
  float s = sinf(angle);
  float one_minus_c = 1.0f - c;

  float x = axis.x;
  float y = axis.y;
  float z = axis.z;

  return (mat4){{c + x * x * one_minus_c, y * x * one_minus_c + z * s, z * x * one_minus_c - y * s, 0.0f,

                 x * y * one_minus_c - z * s, c + y * y * one_minus_c, z * y * one_minus_c + x * s, 0.0f,

                 x * z * one_minus_c + y * s, y * z * one_minus_c - x * s, c + z * z * one_minus_c, 0.0f,

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

  mat4 rotation = {{c + x * x * one_minus_c, y * x * one_minus_c + z * s, z * x * one_minus_c - y * s, 0.0f,

                    x * y * one_minus_c - z * s, c + y * y * one_minus_c, z * y * one_minus_c + x * s, 0.0f,

                    x * z * one_minus_c + y * s, y * z * one_minus_c - x * s, c + z * z * one_minus_c, 0.0f,

                    0.0f, 0.0f, 0.0f, 1.0f}};
  return mat4_multiply(m, rotation);
}

mat4 mat4_translate(mat4 m, vec3 v) {
  mat4 translation = {
      {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, v.x, v.y, v.z, 1.0f}};
  return mat4_multiply(m, translation);
}

mat4 mat4_perspective(float fov, float aspect, float near, float far) {
  float tan_half_fov = tanf(fov * 0.5f);

  return (mat4){{1.0f / (aspect * tan_half_fov), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f,
                 0.0f, 0.0f, -(far + near) / (far - near), -1.0f, 0.0f, 0.0f,
                 -(2.0f * far * near) / (far - near), 0.0f}};
}

mat4 mat4_ortho(float left, float right, float bottom, float top, float near, float far) {
  return (mat4){{2.0f / (right - left), 0.0f, 0.0f, 0.0f, 0.0f, 2.0f / (top - bottom), 0.0f, 0.0f, 0.0f, 0.0f,
                 -2.0f / (far - near), 0.0f, -(right + left) / (right - left),
                 -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1.0f}};
}

rgba_float rgba_div_scalar(rgba rgba, float s) {
  return (rgba_float){rgba.r / s, rgba.g / s, rgba.b / s, rgba.a / s};
}

float to_radian(float degrees) { return degrees * (3.14159265359f / 180.0f); }
