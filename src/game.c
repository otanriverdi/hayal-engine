#include "game.h"
#include "game/asset.h"
#include "game/text.h"
#include "render.h"

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
  text_load_font_glyphs(render_commands, &state->font);
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

  text_render_text(render_commands, (text_render_cmd){
                                        .text = "hello, world!",
                                        .pos = {100.0, 100.0, 0.0},
                                        .color = {255, 255, 255, 255},
                                        .font = &state->font,
                                        .scale = 1,
                                    });
}

void game_deinit(game_memory *memory, render_commands *render_commands) {
  game_state *state = (game_state *)memory->game_state;

  text_delete_font_glyphs(render_commands, &state->font);
  asset_delete_font(&state->font, &memory->allocator);

  renderer_delete_texture(render_commands, (render_command_delete_texture){
                                               .texture_id = &state->sprite.texture_id,
                                           });

  asset_delete_image(&state->sprite, &memory->allocator);
}
