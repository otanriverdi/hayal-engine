#ifndef ALCHEMO_RENDER
#define ALCHEMO_RENDER

#include <assert.h>
#include <stdint.h>

typedef struct {
  float pos[2];
  float size[2];
  uint8_t color[4];
} Rect;

typedef struct {
  Rect *data;
  uint32_t cap;
  uint32_t len;
} RectList;

typedef struct {
  void *asset;
  float pos[2];
  float size[2];
} Sprite;

typedef struct {
  Sprite *data;
  uint32_t cap;
  uint32_t len;
} SpriteList;

typedef struct {
  RectList rects;
  SpriteList sprites;
  uint8_t clear[4];
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
