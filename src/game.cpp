#include "hayal/game.h"
#include "hayal/mem.h"
#include "hayal/platform.h"
#include "hayal/renderer.h"

using namespace hayal;

struct GameState {
  std::vector<char, ArenaAllocator<char>> sample;
};

void GameStateInit(GameState &state, Arena &arena) {
  new (&state.sample)
      std::vector<char, ArenaAllocator<char>>(ArenaAllocator<char>(arena));
}

void hayal::Init(Memory &memory) {
  GameState *game_state = static_cast<GameState *>(
      ArenaAlloc(memory.perma_memory, sizeof(GameState), alignof(GameState)));
  GameStateInit(*game_state, memory.perma_memory);
  memory.game_state = game_state;
};

void hayal::Update(const Input &input, const float dt,
                   RenderCommands &render_commands, SoundBuffer &sound_buffer,
                   Memory &memory) {
  GameState *game_state = static_cast<GameState *>(memory.game_state);
};
