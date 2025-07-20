#ifndef TEXT_H
#define TEXT_H

#include "game/asset.hpp"
#include "renderer.hpp"
#include <glm/glm.hpp>

void text_load_font_glyphs(struct renderer *renderer, asset_font *font);
void text_delete_font_glyphs(struct renderer *renderer, asset_font *font);

struct text_cmd_render {
  asset_font *font;
  const char *text;
  glm::vec3 pos;
  glm::vec4 color;
  float scale;
};
void text_render_text(struct renderer *renderer, text_cmd_render cmd);

#endif
