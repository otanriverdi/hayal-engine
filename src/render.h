#ifndef RENDER_H
#define RENDER_H

#include "asset.h"
#include "math.h"
#include <assert.h>
#include <stdint.h>

typedef enum render_command_type {
  RENDER_COMMAND_CLEAR,
  RENDER_COMMAND_RECT,
  RENDER_COMMAND_SPRITE,
} render_command_type;

typedef struct render_command_clear {
  rgba color;
} render_command_clear;

typedef struct render_command_rect {
  vec3 pos;
  vec2 size;
  rgba color;
} render_command_rect;

typedef struct render_command_sprite {
  loaded_png *asset;
  vec3 pos;
  vec2 size;
} render_command_sprite;

typedef struct render_command {
  render_command_type type;
  union {
    render_command_clear clear;
    render_command_rect rect;
    render_command_sprite sprite;
  };
} render_command;

typedef struct render_commands {
  render_command *data;
  uint32_t cap;
  uint32_t len;
  vec2 camera_pos;
} render_commands;

render_commands render_commands_init(uint64_t size);
void render_commands_free(render_commands *commands);
void render_commands_clear(render_commands *commands);

void render_clear(render_commands *commands, render_command_clear clear);
void render_rect(render_commands *commands, render_command_rect rect);
void render_sprite(render_commands *commands, render_command_sprite sprite);

struct renderer;
struct renderer renderer_init(int framebuffer_width, int framebuffer_height);
void renderer_destroy(struct renderer *renderer);
void renderer_process_commands(struct renderer *renderer,
                               render_commands *commands);

#endif
