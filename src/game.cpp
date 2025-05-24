#include "hayal/game.h"
#include "glm/fwd.hpp"
#include "hayal/mem.h"
#include "hayal/platform.h"
#include "hayal/renderer.h"
#include "stb_image.h"

using namespace hayal;

struct GameState {
  SpriteAsset asset;
};

void hayal::Init(Memory &memory) {
  GameState *game_state = static_cast<GameState *>(
      ArenaAlloc(memory.perma_memory, sizeof(GameState), alignof(GameState)));

  auto allocator = ArenaAllocator<unsigned char>(memory.temp_memory);
  auto image =
      platform::ReadEntireFile("assets/wizard-idle.png", allocator).value();

  int x, y, n;
  game_state->asset.data = stbi_load_from_memory(
      image.data(), static_cast<int>(image.size()), &x, &y, &n, 4);
  game_state->asset.size.x = x;
  game_state->asset.size.y = y;

  memory.game_state = game_state;
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
