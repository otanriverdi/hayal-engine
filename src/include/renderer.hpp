#ifndef RENDER_H
#define RENDER_H

#include "mem.hpp"
#include <assert.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdint.h>

struct render_cmd_quad {
  uint32_t texture_id;
  glm::vec3 pos;
  glm::vec2 size;
  glm::vec4 color;
};

struct render_cmd_glyph {
  uint32_t texture_id;
  glm::vec3 pos;
  glm::vec2 size;
  glm::vec4 color;
};

struct render_cmd_delete_texture {
  uint32_t *texture_id;
};

struct render_cmd_load_texture {
  uint32_t *texture_id;
  uint8_t *data;
  glm::vec2 size;
};

struct render_cmd_load_glyph {
  uint32_t *texture_id;
  uint8_t *data;
  glm::vec2 size;
};

struct renderer;
struct renderer renderer_init(int framebuffer_width, int framebuffer_height, mem_allocator *temp_allocator);
void renderer_destroy(struct renderer *renderer);

void renderer_begin_frame(struct renderer *renderer);
void renderer_render_clear(struct renderer *renderer, glm::vec4 clear);
void renderer_render_quad(struct renderer *renderer, render_cmd_quad quad);
void renderer_render_glyph(struct renderer *renderer, render_cmd_glyph glyph);
void renderer_delete_texture(struct renderer *renderer, render_cmd_delete_texture delete_texture);
void renderer_load_texture(struct renderer *renderer, render_cmd_load_texture load_texture);
void renderer_load_glyph(struct renderer *renderer, render_cmd_load_glyph load_glyph);
void renderer_move_camera(struct renderer *renderer, glm::vec2 delta);

#endif
