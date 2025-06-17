#include "math.h"
#include <math.h>

Vec2 Vec2Add(Vec2 a, Vec2 b) { return (Vec2){a.x + b.x, a.y + b.y}; }

Vec2 Vec2Sub(Vec2 a, Vec2 b) { return (Vec2){a.x - b.x, a.y - b.y}; }

Vec2 Vec2Mul(Vec2 a, Vec2 b) { return (Vec2){a.x * b.x, a.y * b.y}; }

Vec2 Vec2Div(Vec2 a, Vec2 b) { return (Vec2){a.x / b.x, a.y / b.y}; }

Vec2 Vec2AddScalar(Vec2 v, float s) { return (Vec2){v.x + s, v.y + s}; }

Vec2 Vec2SubScalar(Vec2 v, float s) { return (Vec2){v.x - s, v.y - s}; }

Vec2 Vec2MulScalar(Vec2 v, float s) { return (Vec2){v.x * s, v.y * s}; }

Vec2 Vec2DivScalar(Vec2 v, float s) { return (Vec2){v.x / s, v.y / s}; }

float Vec2Dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }

float Vec2Length(Vec2 v) { return sqrtf(v.x * v.x + v.y * v.y); }

float Vec2LengthSq(Vec2 v) { return v.x * v.x + v.y * v.y; }

Vec2 Vec2Normalize(Vec2 v) {
  float len = Vec2Length(v);
  if (len == 0.0f) {
    return (Vec2){0.0f, 0.0f};
  }
  return (Vec2){v.x / len, v.y / len};
}

RGBAf RGBADivScalar(RGBA rgba, float s) {
  return (RGBAf){rgba.r / s, rgba.g / s, rgba.b / s, rgba.a / s};
}
