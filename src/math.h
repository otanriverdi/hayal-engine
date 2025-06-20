#ifndef HAYAL_MATH
#define HAYAL_MATH

#include <stdint.h>

typedef struct {
  float x, y;
} Vec2;

typedef struct {
  uint8_t r, g, b, a;
} RGBA;

typedef struct {
  float r, g, b, a;
} RGBAf;

Vec2 Vec2Add(Vec2 a, Vec2 b);
Vec2 Vec2Sub(Vec2 a, Vec2 b);
Vec2 Vec2Mul(Vec2 a, Vec2 b);
Vec2 Vec2Div(Vec2 a, Vec2 b);

Vec2 Vec2AddScalar(Vec2 v, float s);
Vec2 Vec2SubScalar(Vec2 v, float s);
Vec2 Vec2MulScalar(Vec2 v, float s);
Vec2 Vec2DivScalar(Vec2 v, float s);

float Vec2Dot(Vec2 a, Vec2 b);
float Vec2Length(Vec2 v);
float Vec2LengthSq(Vec2 v);
Vec2 Vec2Normalize(Vec2 v);

RGBAf RGBADivScalar(RGBA rgba, float s);

#endif
