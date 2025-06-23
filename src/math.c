#include "math.h"
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

rgba_float rgba_div_scalar(rgba rgba, float s) {
  return (rgba_float){rgba.r / s, rgba.g / s, rgba.b / s, rgba.a / s};
}
