#ifndef RENDER_H
#define RENDER_H

#include "linalg.h"
#include <assert.h>
#include <stdint.h>

struct render_cmd_quad {
  uint32_t texture_id;
  vec3 pos;
  vec2 size;
  rgba color;
};

struct render_cmd_glyph {
  uint32_t texture_id;
  vec3 pos;
  vec2 size;
  rgba color;
};

struct render_cmd_delete_texture {
  uint32_t *texture_id;
};

struct render_cmd_load_texture {
  uint32_t *texture_id;
  uint8_t *data;
  vec2 size;
};

struct render_cmd_load_glyph {
  uint32_t *texture_id;
  uint8_t *data;
  vec2 size;
};

struct renderer;
struct renderer renderer_init(int framebuffer_width, int framebuffer_height);
void renderer_destroy(struct renderer *renderer);

void renderer_render_clear(struct renderer *renderer, rgba clear);
void renderer_render_quad(struct renderer *renderer, render_cmd_quad quad);
void renderer_render_glyph(struct renderer *renderer, render_cmd_glyph glyph);
void renderer_delete_texture(struct renderer *renderer, render_cmd_delete_texture delete_texture);
void renderer_load_texture(struct renderer *renderer, render_cmd_load_texture load_texture);
void renderer_load_glyph(struct renderer *renderer, render_cmd_load_glyph load_glyph);
void renderer_move_camera(struct renderer *renderer, vec2 delta);

#endif
