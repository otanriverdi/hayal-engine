#ifndef TEXT_H
#define TEXT_H

#include "game/asset.h"
#include "linalg.h"
#include "renderer.h"

void text_load_font_glyphs(struct renderer *renderer, asset_font *font);
void text_delete_font_glyphs(struct renderer *renderer, asset_font *font);

typedef struct text_cmd_render {
  asset_font *font;
  char *text;
  vec3 pos;
  rgba color;
  float scale;
} text_cmd_render;
void text_render_text(struct renderer *renderer, text_cmd_render cmd);

#endif
