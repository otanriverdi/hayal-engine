#include "game.h"
#include "asset.h"
#include "render.h"

typedef struct game_state {
  loaded_png sprite;
  loaded_wav wav;
} game_state;

void game_init(game_memory *memory) {
  game_state *state = (game_state *)memory->perma_memory;
  state->sprite = load_png("assets/wizard-idle.png", &memory->free_list,
                           &memory->temp_memory);
  state->wav = load_wav("assets/coin.wav", 2, 48000, &memory->free_list,
                        &memory->temp_memory);
}

void game_update(const game_input *input, const float dt, game_memory *memory,
                 render_commands *render_commands) {
  game_state *state = (game_state *)memory->perma_memory;

  uint8_t clear[4] = {255, 255, 255, 255};
  render_clear(render_commands, clear);

  rect rect = {
      .pos = {10.0, 10.0}, .color = {71, 99, 255, 255}, .size = {100.0, 100.0}};
  render_rect(render_commands, rect);

  sprite sprite = {
      .pos = {200.0, 200.0},
      .asset = &state->sprite,
      .size = state->sprite.size,
  };
  render_sprite(render_commands, sprite);
}
