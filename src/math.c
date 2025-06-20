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

rgba_float rgba_div_scalar(rgba rgba, float s) {
  return (rgba_float){rgba.r / s, rgba.g / s, rgba.b / s, rgba.a / s};
}
