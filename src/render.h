#ifndef RENDER_H
#define RENDER_H

#include "asset.h"
#include "math.h"
#include <assert.h>
#include <stdint.h>

typedef struct rect {
  uint32_t texture_id;
  vec3 pos;
  vec2 size;
  rgba color;
} rect;

typedef struct rect_list {
  rect *data;
  uint32_t cap;
  uint32_t len;
} rect_list;

typedef struct sprite {
  loaded_png *asset;
  vec3 pos;
  vec2 size;
} sprite;

typedef struct sprite_list {
  sprite *data;
  uint32_t cap;
  uint32_t len;
} sprite_list;

typedef struct render_commands {
  rect_list rects;
  sprite_list sprites;
  rgba clear;
} render_commands;

render_commands render_commands_init(uint64_t size_mb);
void render_commands_free(render_commands *commands);
void render_commands_clear(render_commands *commands);

void render_rect(render_commands *commands, rect rect);
void render_sprite(render_commands *commands, sprite sprite);
void render_clear(render_commands *commands, uint8_t color[4]);

struct renderer;
struct renderer renderer_init(int framebuffer_width, int framebuffer_height);
void renderer_destroy(struct renderer *renderer);
void renderer_process_commands(struct renderer *renderer,
                               render_commands *commands);

#endif
