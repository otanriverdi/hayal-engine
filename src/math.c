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

vec3 vec3_scale(vec3 v, float s) { return (vec3){v.x * s, v.y * s, v.z * s}; }

vec3 vec3_translate(vec3 v, vec3 t) {
  return (vec3){v.x + t.x, v.y + t.y, v.z + t.z};
}

vec4 vec4_scale(vec4 v, float s) {
  return (vec4){v.x * s, v.y * s, v.z * s, v.w * s};
}

vec4 vec4_translate(vec4 v, vec4 t) {
  return (vec4){v.x + t.x, v.y + t.y, v.z + t.z, v.w + t.w};
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

mat3 mat3_translation(float x, float y) {
  return (mat3){{1.0f, 0.0f, x, 0.0f, 1.0f, y, 0.0f, 0.0f, 1.0f}};
}

mat3 mat3_rotation(float angle) {
  float c = cosf(angle);
  float s = sinf(angle);
  return (mat3){{c, -s, 0.0f, s, c, 0.0f, 0.0f, 0.0f, 1.0f}};
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

mat4 mat4_translation(vec3 t) {
  return (mat4){{1.0f, 0.0f, 0.0f, t.x, 0.0f, 1.0f, 0.0f, t.y, 0.0f, 0.0f, 1.0f,
                 t.z, 0.0f, 0.0f, 0.0f, 1.0f}};
}

mat4 mat4_rotation_x(float angle) {
  float c = cosf(angle);
  float s = sinf(angle);
  return (mat4){{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c, -s, 0.0f, 0.0f, s, c, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f}};
}

mat4 mat4_rotation_y(float angle) {
  float c = cosf(angle);
  float s = sinf(angle);
  return (mat4){{c, 0.0f, s, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -s, 0.0f, c, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f}};
}

mat4 mat4_rotation_z(float angle) {
  float c = cosf(angle);
  float s = sinf(angle);
  return (mat4){{c, -s, 0.0f, 0.0f, s, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f}};
}

mat4 mat4_perspective(float fov, float aspect, float near, float far) {
  float f = 1.0f / tanf(fov * 0.5f);
  float range_inv = 1.0f / (near - far);

  return (mat4){{f / aspect, 0.0f, 0.0f, 0.0f, 0.0f, f, 0.0f, 0.0f, 0.0f, 0.0f,
                 (near + far) * range_inv, 2.0f * near * far * range_inv, 0.0f,
                 0.0f, -1.0f, 0.0f}};
}

mat4 mat4_look_at(vec3 eye, vec3 center, vec3 up) {
  vec3 f = vec3_normalize(vec3_sub(center, eye));
  vec3 s = vec3_normalize(vec3_cross(f, up));
  vec3 u = vec3_cross(s, f);

  return (mat4){{s.x, u.x, -f.x, 0.0f, s.y, u.y, -f.y, 0.0f, s.z, u.z, -f.z,
                 0.0f, -vec3_dot(s, eye), -vec3_dot(u, eye), vec3_dot(f, eye),
                 1.0f}};
}

vec3 vec3_transform_mat3(vec3 v, mat3 m) {
  return (vec3){m.data[0] * v.x + m.data[3] * v.y + m.data[6] * v.z,
                m.data[1] * v.x + m.data[4] * v.y + m.data[7] * v.z,
                m.data[2] * v.x + m.data[5] * v.y + m.data[8] * v.z};
}

vec3 vec3_transform_mat4(vec3 v, mat4 m) {
  float w = m.data[3] * v.x + m.data[7] * v.y + m.data[11] * v.z + m.data[15];
  if (w == 0.0f)
    w = 1.0f;
  return (vec3){
      (m.data[0] * v.x + m.data[4] * v.y + m.data[8] * v.z + m.data[12]) / w,
      (m.data[1] * v.x + m.data[5] * v.y + m.data[9] * v.z + m.data[13]) / w,
      (m.data[2] * v.x + m.data[6] * v.y + m.data[10] * v.z + m.data[14]) / w};
}

vec4 vec4_transform_mat4(vec4 v, mat4 m) {
  return (vec4){
      m.data[0] * v.x + m.data[4] * v.y + m.data[8] * v.z + m.data[12] * v.w,
      m.data[1] * v.x + m.data[5] * v.y + m.data[9] * v.z + m.data[13] * v.w,
      m.data[2] * v.x + m.data[6] * v.y + m.data[10] * v.z + m.data[14] * v.w,
      m.data[3] * v.x + m.data[7] * v.y + m.data[11] * v.z + m.data[15] * v.w};
}

rgba_float rgba_div_scalar(rgba rgba, float s) {
  return (rgba_float){rgba.r / s, rgba.g / s, rgba.b / s, rgba.a / s};
}
