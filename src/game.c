#include "game.h"
#include "asset.h"
#include "render.h"

typedef struct game_state {
  loaded_png sprite;
} game_state;

void game_init(game_memory *memory) {
  game_state *state = (game_state *)memory->perma_memory;
  state->sprite = load_png("assets/wizard-idle.png", &memory->free_list,
                           &memory->temp_memory);
}

void game_update(const game_input *input, const float dt, game_memory *memory,
                 render_commands *render_commands) {
  game_state *state = (game_state *)memory->perma_memory;

  uint8_t clear[4] = {255, 255, 255, 255};
  render_clear(render_commands, clear);

  sprite sprite = {
      .pos = {200.0, 200.0},
      .asset = &state->sprite,
      .size = state->sprite.size,
  };
  render_sprite(render_commands, sprite);
}
