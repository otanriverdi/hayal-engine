#ifndef TEXT_H
#define TEXT_H

#include "game/asset.h"
#include "linalg.h"
#include "render.h"

void text_load_font_glyphs(render_commands *render_commands, asset_font *font);
void text_delete_font_glyphs(render_commands *render_commands, asset_font *font);

typedef struct text_render_cmd {
  asset_font *font;
  char *text;
  vec3 pos;
  rgba color;
  float scale;
} text_render_cmd;
void text_render_text(render_commands *render_commands, text_render_cmd cmd);

#endif
