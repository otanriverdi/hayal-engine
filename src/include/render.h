#ifndef RENDER_H
#define RENDER_H

#include "linalg.h"
#include <assert.h>
#include <stdint.h>

typedef enum render_command_type {
  RENDER_COMMAND_CLEAR,
  RENDER_COMMAND_QUAD,
  RENDER_COMMAND_GLYPH,
  RENDER_COMMAND_DELETE_TEXTURE,
  RENDER_COMMAND_LOAD_TEXTURE,
  RENDER_COMMAND_LOAD_GLYPH,
} render_command_type;

typedef struct render_command_clear {
  rgba color;
} render_command_clear;

typedef struct render_command_quad {
  uint32_t texture_id;
  vec3 pos;
  vec2 size;
  rgba color;
} render_command_quad;

typedef struct render_command_glyph {
  uint32_t texture_id;
  vec3 pos;
  vec2 size;
  rgba color;
} render_command_glyph;

typedef struct render_command_delete_texture {
  uint32_t *texture_id;
} render_command_delete_texture;

typedef struct render_command_load_texture {
  uint32_t *texture_id;
  uint8_t *data;
  vec2 size;
} render_command_load_texture;

typedef struct render_command_load_font_texture {
  uint32_t *texture_id;
  uint8_t *data;
  vec2 size;
} render_command_load_glyph;

typedef struct render_command {
  render_command_type type;
  union {
    render_command_clear clear;
    render_command_quad quad;
    render_command_glyph glyph;
    render_command_delete_texture delete_texture;
    render_command_load_texture load_texture;
    render_command_load_glyph load_glyph;
  };
} render_command;

#define MAX_RENDER_COMMANDS 1024

typedef struct render_commands {
  render_command data[MAX_RENDER_COMMANDS];
  uint32_t len;
  vec2 camera_pos;
} render_commands;

void render_commands_clear(render_commands *commands);

void renderer_render_clear(render_commands *commands, render_command_clear clear);
void renderer_render_quad(render_commands *commands, render_command_quad quad);
void renderer_render_glyph(render_commands *commands, render_command_glyph glyph);
void renderer_delete_texture(render_commands *commands, render_command_delete_texture delete_texture);
void renderer_load_texture(render_commands *commands, render_command_load_texture load_texture);
void renderer_load_glyph(render_commands *commands, render_command_load_glyph load_texture);

struct renderer;
struct renderer renderer_init(int framebuffer_width, int framebuffer_height);
void renderer_destroy(struct renderer *renderer);
void renderer_process_commands(struct renderer *renderer, render_commands *commands);

#endif
