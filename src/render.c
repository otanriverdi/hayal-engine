#include "render.h"
#include <stdint.h>
#include <stdlib.h>

static rect_list rect_list_init(uint32_t cap) {
  return (rect_list){
      .data = malloc(cap * sizeof(rect)),
      .cap = cap,
      .len = 0,
  };
};

static int rect_list_push(rect_list *list, rect rect) {
  if (list->len == list->cap) {
    return -1;
  }
  list->data[list->len++] = rect;
  return list->len - 1;
}

static void rect_list_free(rect_list *list) {
  free(list->data);
  list->cap = 0;
  list->len = 0;
}

static sprite_list sprite_list_init(uint32_t cap) {
  return (sprite_list){
      .data = malloc(cap * sizeof(sprite)),
      .cap = cap,
      .len = 0,
  };
};

static int sprite_list_push(sprite_list *list, sprite rect) {
  if (list->len == list->cap) {
    return -1;
  }
  list->data[list->len++] = rect;
  return list->len - 1;
}

static void sprite_list_free(sprite_list *list) {
  free(list->data);
  list->cap = 0;
  list->len = 0;
}

render_commands render_commands_init(uint64_t unit_cap) {
  return (render_commands){.rects = rect_list_init(unit_cap),
                           .sprites = sprite_list_init(unit_cap)};
}

void render_commands_clear(render_commands *commands) {
  commands->rects.len = 0;
  commands->sprites.len = 0;
}

void render_rect(render_commands *commands, rect rect) {
  rect_list_push(&commands->rects, rect);
}

void render_sprite(render_commands *commands, sprite sprite) {
  sprite_list_push(&commands->sprites, sprite);
}

void render_clear(render_commands *commands, uint8_t color[4]) {
  commands->clear.r = color[0];
  commands->clear.g = color[1];
  commands->clear.b = color[2];
  commands->clear.a = color[3];
}

void render_commands_free(render_commands *commands) {
  sprite_list_free(&commands->sprites);
  rect_list_free(&commands->rects);
}
