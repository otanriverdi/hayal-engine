#ifndef MATH_H
#define MATH_H

#include <stdint.h>

typedef struct vec2 {
  float x, y;
} vec2;

typedef struct rgba {
  uint8_t r, g, b, a;
} rgba;

typedef struct rgba_float {
  float r, g, b, a;
} rgba_float;

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

rgba_float rgba_div_scalar(rgba rgba, float s);

#endif
