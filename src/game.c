#include "game.h"
#include "render.h"

void GameInit(GameMemory *memory) {}

void GameUpdate(const GameInput *input, const float dt, GameMemory *memory,
                RenderCommands *render_commands) {
  uint8_t clear[4] = {255, 165, 0, 255};
  RenderClear(render_commands, clear);

  Rect rect = {
      .pos = {10.0, 10.0}, .color = {71, 99, 255, 255}, .size = {100.0, 100.0}};
  RenderRect(render_commands, rect);
}
