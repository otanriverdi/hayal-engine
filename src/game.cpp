#include "hayal/game.h"
#include "glm/fwd.hpp"
#include "hayal/asset.h"
#include "hayal/mem.h"
#include "hayal/renderer.h"
#include "stb_image.h"

using namespace hayal;

struct GameState {
  Image asset;
};

void hayal::Init(Memory &memory) {
  GameState *game_state = static_cast<GameState *>(
      ArenaAlloc(memory.perma_memory, sizeof(GameState), alignof(GameState)));
  memory.game_state = game_state;

  game_state->asset =
      LoadPng("assets/wizard-idle.png", memory.perma_memory).value();
};

void hayal::Update(const Input &input, const float dt,
                   RenderCommands &render_commands, SoundBuffer &sound_buffer,
                   Memory &memory) {
  GameState *game_state = static_cast<GameState *>(memory.game_state);

  Sprite s = {};
  s.asset = &game_state->asset;
  s.pos = {10.0, 10.0};
  s.size = game_state->asset.size;
  render_commands.sprites.push_back(s);
};
