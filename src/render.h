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
  RENDER_COMMAND_DELETE_TEXTURE,
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
  rgba color;
} render_command_sprite;

typedef struct render_command_delete_texture {
  uint32_t texture_id;
} render_command_delete_texture;

typedef struct render_command {
  render_command_type type;
  union {
    render_command_clear clear;
    render_command_rect rect;
    render_command_sprite sprite;
    render_command_delete_texture delete_texture;
  };
} render_command;

#define MAX_RENDER_COMMANDS 1024

typedef struct render_commands {
  render_command data[MAX_RENDER_COMMANDS];
  uint32_t len;
  vec2 camera_pos;
} render_commands;

void render_commands_clear(render_commands *commands);

void render_clear(render_commands *commands, render_command_clear clear);
void render_rect(render_commands *commands, render_command_rect rect);
void render_sprite(render_commands *commands, render_command_sprite sprite);
void delete_texture(render_commands *commands, render_command_delete_texture delete_texture);

struct renderer;
struct renderer renderer_init(int framebuffer_width, int framebuffer_height);
void renderer_destroy(struct renderer *renderer);
void renderer_process_commands(struct renderer *renderer, render_commands *commands);

#endif
