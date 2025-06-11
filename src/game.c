#include "game.h"
#include "render.h"

void GameInit(GameMemory *memory) {}

void GameUpdate(const GameInput *input, const float dt, GameMemory *memory,
                RenderCommands *render_commands) {
  uint8_t clear[4] = {255, 165, 0, 255};
  RenderClear(render_commands, clear);
}
