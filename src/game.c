#include "game.h"
#include "render.h"

void game_init(game_memory *memory) {}

void game_update(const game_input *input, const float dt, game_memory *memory,
                 render_commands *render_commands) {
  uint8_t clear[4] = {255, 165, 0, 255};
  render_clear(render_commands, clear);

  rect rect = {
      .pos = {10.0, 10.0}, .color = {71, 99, 255, 255}, .size = {100.0, 100.0}};
  render_rect(render_commands, rect);
}
