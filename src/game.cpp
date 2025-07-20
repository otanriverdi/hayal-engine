#include "game.hpp"
#include "game/asset.hpp"
#include "game/text.hpp"
#include "renderer.hpp"

struct game_state {
  asset_image sprite;
  asset_font font;
};

void game_init(game_memory *memory, struct renderer *renderer) {
  game_state *state = (game_state *)memory->game_state;
  state->sprite = asset_load_image("assets/wizard-idle.png", &memory->allocator, &memory->temp_allocator);
  renderer_load_texture(renderer, (render_cmd_load_texture){
                                      .texture_id = &state->sprite.texture_id,
                                      .data = state->sprite.data,
                                      .size = state->sprite.size,
                                  });

  state->font = asset_load_font("assets/Roboto.ttf", 48.0f, &memory->allocator, &memory->temp_allocator);
  text_load_font_glyphs(renderer, &state->font);
}

void game_update(const game_input *input, const float dt, game_memory *memory, struct renderer *renderer) {
  renderer_begin_frame(renderer);
  game_state *state = (game_state *)memory->game_state;

  float camera_speed = 500.0f * dt;
  if (input->keys[KEY_W].is_down) {
    renderer_move_camera(renderer, glm::vec2(0.0f, -camera_speed));
  }
  if (input->keys[KEY_S].is_down) {
    renderer_move_camera(renderer, glm::vec2(0.0f, +camera_speed));
  }
  if (input->keys[KEY_A].is_down) {
    renderer_move_camera(renderer, glm::vec2(camera_speed, 0.0f));
  }
  if (input->keys[KEY_D].is_down) {
    renderer_move_camera(renderer, glm::vec2(-camera_speed, 0.0f));
  }

  renderer_render_clear(renderer, glm::vec4(51, 77, 77, 255));

  renderer_render_quad(
      renderer, (render_cmd_quad){.pos = {20.0, 20.0, 0.0}, .size = {20.0, 20.0}, .color = {255, 0, 0, 255}});

  renderer_render_quad(renderer, (render_cmd_quad){.texture_id = state->sprite.texture_id,
                                                   .pos = {1920.0 / 2, 1080.0 / 2, 0.0},
                                                   .size = {state->sprite.size.x, state->sprite.size.y}});

  text_render_text(renderer, (text_cmd_render){
                                 .font = &state->font,
                                 .text = "hello, world!",
                                 .pos = {100.0, 100.0, 0.0},
                                 .color = {255, 255, 255, 255},
                                 .scale = 1,
                             });
}

void game_deinit(game_memory *memory, struct renderer *renderer) {
  game_state *state = (game_state *)memory->game_state;

  text_delete_font_glyphs(renderer, &state->font);
  asset_delete_font(&state->font, &memory->allocator);

  renderer_delete_texture(renderer, (render_cmd_delete_texture){
                                        .texture_id = &state->sprite.texture_id,
                                    });

  asset_delete_image(&state->sprite, &memory->allocator);
}
