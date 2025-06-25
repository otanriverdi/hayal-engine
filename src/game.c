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

  uint8_t clear[4] = {51, 77, 77, 255};
  render_clear(render_commands, clear);

  sprite sprite = {
      .pos = {1920.0 / 2, 1080.0 / 2, 0.0},
      .asset = &state->sprite,
      .size = {state->sprite.size.x, state->sprite.size.y},
  };
  render_sprite(render_commands, sprite);
}
