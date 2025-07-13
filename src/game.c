#include "game.h"
#include "game/asset.h"
#include "render.h"
#include <string.h>

typedef struct game_state {
  asset_image sprite;
  asset_font font;
} game_state;

void game_init(game_memory *memory, render_commands *render_commands) {
  game_state *state = (game_state *)memory->game_state;
  state->sprite = asset_load_image("assets/wizard-idle.png", &memory->allocator, &memory->temp_allocator);
  renderer_load_texture(render_commands, (render_command_load_texture){
                                             .data = state->sprite.data,
                                             .texture_id = &state->sprite.texture_id,
                                             .size = state->sprite.size,
                                         });

  state->font = asset_load_font("assets/Roboto.ttf", 48.0f, &memory->allocator, &memory->temp_allocator);
  for (int i = 0; i < ASSET_FONT_NUM_CHARS; i++) {
    renderer_load_glyph(render_commands, (render_command_load_glyph){
                                             .texture_id = &state->font.characters[i].texture_id,
                                             .data = state->font.characters[i].data,
                                             .size = state->font.characters[i].size,
                                         });
  }
}

void game_update(const game_input *input, const float dt, game_memory *memory,
                 render_commands *render_commands) {
  game_state *state = (game_state *)memory->game_state;

  float camera_speed = 500.0f * dt;
  if (input->keys[KEY_W].is_down) {
    render_commands->camera_pos.y -= camera_speed;
  }
  if (input->keys[KEY_S].is_down) {
    render_commands->camera_pos.y += camera_speed;
  }
  if (input->keys[KEY_A].is_down) {
    render_commands->camera_pos.x += camera_speed;
  }
  if (input->keys[KEY_D].is_down) {
    render_commands->camera_pos.x -= camera_speed;
  }

  renderer_render_clear(render_commands, (render_command_clear){.color = {51, 77, 77, 255}});

  renderer_render_quad(
      render_commands,
      (render_command_quad){.pos = {20.0, 20.0, 0.0}, .size = {20.0, 20.0}, .color = {255, 0, 0, 255}});

  renderer_render_quad(render_commands,
                       (render_command_quad){.texture_id = state->sprite.texture_id,
                                             .pos = {1920.0 / 2, 1080.0 / 2, 0.0},
                                             .size = {state->sprite.size.x, state->sprite.size.y}});

  char *text = "hello, world";
  vec3 text_pos = {100.0, 100.0, 0.0};
  rgba text_color = {255, 255, 255, 255};

  float scale = 1.0f;
  float x = text_pos.x;
  for (int i = 0; i < strlen(text); i++) {
    unsigned char c = (unsigned char)text[i];
    asset_font_char *ch = &state->font.characters[c];
    float w = ch->size.x * scale;
    float h = ch->size.y * scale;
    float xpos = x + ch->bearing.x * scale + w / 2.0f;
    float ypos = text_pos.y - (ch->size.y - ch->bearing.y) * scale + h / 2.0f;
    vec3 char_pos = {xpos, ypos, text_pos.z};
    vec2 char_size = {w, h};
    renderer_render_glyph(render_commands, (render_command_glyph){.texture_id = ch->texture_id,
                                                                  .pos = char_pos,
                                                                  .size = char_size,
                                                                  .color = text_color});
    x += (ch->advance >> 6) * scale;
  }
}

void game_deinit(game_memory *memory, render_commands *render_commands) {
  game_state *state = (game_state *)memory->game_state;

  for (int i = 0; i < ASSET_FONT_NUM_CHARS; i++) {
    if (state->font.characters[i].texture_id > 0) {
      renderer_delete_texture(render_commands, (render_command_delete_texture){
                                                   .texture_id = &state->font.characters[i].texture_id,
                                               });
    }
  }
  asset_delete_font(&state->font, &memory->allocator);

  if (state->sprite.texture_id > 0) {
    renderer_delete_texture(render_commands, (render_command_delete_texture){
                                                 .texture_id = &state->sprite.texture_id,
                                             });
  }
  asset_delete_image(&state->sprite, &memory->allocator);
}
