#include "render.h"
#include <stdint.h>
#include <stdlib.h>

static RectList RectListInit(uint32_t cap) {
  return (RectList){
      .data = malloc(cap * sizeof(Rect)),
      .cap = cap,
      .len = 0,
  };
};

static int RectListPush(RectList *list, Rect rect) {
  if (list->len == list->cap) {
    return -1;
  }
  list->data[list->len++] = rect;
  return list->len - 1;
}

static void RectListFree(RectList *list) {
  free(list->data);
  list->cap = 0;
  list->len = 0;
}

static SpriteList SpriteListInit(uint32_t cap) {
  return (SpriteList){
      .data = malloc(cap * sizeof(Sprite)),
      .cap = cap,
      .len = 0,
  };
};

static int SpriteListPush(SpriteList *list, Sprite rect) {
  if (list->len == list->cap) {
    return -1;
  }
  list->data[list->len++] = rect;
  return list->len - 1;
}

static void SpriteListFree(SpriteList *list) {
  free(list->data);
  list->cap = 0;
  list->len = 0;
}

RenderCommands RenderCommandsInit(uint64_t unit_cap) {
  return (RenderCommands){.rects = RectListInit(unit_cap),
                          .sprites = SpriteListInit(unit_cap)};
}

void RenderCommandsClear(RenderCommands *commands) {
  commands->rects.len = 0;
  commands->sprites.len = 0;
}

void RenderRect(RenderCommands *commands, Rect rect) {
  RectListPush(&commands->rects, rect);
}

void RenderSprite(RenderCommands *commands, Sprite sprite) {
  SpriteListPush(&commands->sprites, sprite);
}

void RenderClear(RenderCommands *commands, uint8_t color[4]) {
  commands->clear[0] = color[0];
  commands->clear[1] = color[1];
  commands->clear[2] = color[2];
  commands->clear[3] = color[3];
}

void RenderCommandsFree(RenderCommands *commands) {
  SpriteListFree(&commands->sprites);
  RectListFree(&commands->rects);
}
