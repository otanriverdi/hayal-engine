#include "game.h"
#include "asset.h"
#include "render.h"

typedef struct game_state {
  loaded_png sprite;
} game_state;

void game_init(game_memory *memory) {
  game_state *state = (game_state *)memory->perma_memory;
  state->sprite = load_image("assets/wizard-idle.png", &memory->free_list,
                             &memory->temp_memory);
}

void game_update(const game_input *input, const float dt, game_memory *memory,
                 render_commands *render_commands) {
  game_state *state = (game_state *)memory->perma_memory;

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

  render_clear(render_commands,
               (render_command_clear){.color = {51, 77, 77, 255}});

  render_rect(render_commands,
              (render_command_rect){.pos = {20.0, 20.0, 0.0},
                                    .size = {20.0, 20.0},
                                    .color = {255, 0, 0, 255}});

  render_sprite(render_commands,
                (render_command_sprite){
                    .asset = &state->sprite,
                    .pos = {1920.0 / 2, 1080.0 / 2, 0.0},
                    .size = {state->sprite.size.x, state->sprite.size.y}});
}
