#include "game/text.h"
#include "linalg.h"
#include <string.h>

void text_load_font_glyphs(struct renderer *renderer, asset_font *font) {
  for (int i = 0; i < ASSET_FONT_NUM_CHARS; i++) {
    renderer_load_glyph(renderer, (render_cmd_load_glyph){
                                      .texture_id = &font->characters[i].texture_id,
                                      .data = font->characters[i].data,
                                      .size = font->characters[i].size,
                                  });
  }
}

void text_delete_font_glyphs(struct renderer *renderer, asset_font *font) {
  for (int i = 0; i < ASSET_FONT_NUM_CHARS; i++) {
    if (font->characters[i].texture_id > 0) {
      renderer_delete_texture(renderer, (render_cmd_delete_texture){
                                            .texture_id = &font->characters[i].texture_id,
                                        });
    }
  }
}

void text_render_text(struct renderer *renderer, text_render_cmd cmd) {
  float x = cmd.pos.x;
  for (int i = 0; i < strlen(cmd.text); i++) {
    unsigned char c = (unsigned char)cmd.text[i];
    asset_font_char *ch = &cmd.font->characters[c];
    float w = ch->size.x * cmd.scale;
    float h = ch->size.y * cmd.scale;
    float xpos = x + ch->bearing.x * cmd.scale + w / 2.0f;
    float ypos = cmd.pos.y - (ch->size.y - ch->bearing.y) * cmd.scale + h / 2.0f;
    vec3 char_pos = {xpos, ypos, cmd.pos.z};
    vec2 char_size = {w, h};
    renderer_render_glyph(
        renderer, (render_cmd_glyph){
                      .texture_id = ch->texture_id, .pos = char_pos, .size = char_size, .color = cmd.color});
    x += (ch->advance >> 6) * cmd.scale;
  }
}
