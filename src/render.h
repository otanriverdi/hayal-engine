#ifndef HAYAL_RENDER
#define HAYAL_RENDER

#include "math.h"
#include <assert.h>
#include <stdint.h>

typedef struct {
  Vec2 pos;
  Vec2 size;
  RGBA color;
} Rect;

typedef struct {
  Rect *data;
  uint32_t cap;
  uint32_t len;
} RectList;

typedef struct {
  void *asset;
  Vec2 pos;
  Vec2 size;
} Sprite;

typedef struct {
  Sprite *data;
  uint32_t cap;
  uint32_t len;
} SpriteList;

typedef struct {
  RectList rects;
  SpriteList sprites;
  RGBA clear;
} RenderCommands;

RenderCommands RenderCommandsInit(uint64_t size_mb);
void RenderCommandsFree(RenderCommands *commands);
void RenderCommandsClear(RenderCommands *commands);

void RenderRect(RenderCommands *commands, Rect rect);
void RenderSprite(RenderCommands *commands, Sprite sprite);
void RenderClear(RenderCommands *commands, uint8_t color[4]);

typedef struct Renderer Renderer;
Renderer RendererInit(int framebuffer_width, int framebuffer_height);
void RendererDestroy(Renderer *renderer);
void RendererProcessCommands(Renderer *renderer, RenderCommands *commands);

#endif
