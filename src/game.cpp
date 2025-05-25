#include "hayal/game.h"
#include "glm/fwd.hpp"
#include "hayal/asset.h"
#include "hayal/mem.h"
#include "hayal/renderer.h"
#include "stb_image.h"
#include <cstdint>

using namespace hayal;

struct GameState {
  Image asset;
  Sound sound;
  uintptr_t sound_cursor = 0;
};

void hayal::Init(Memory &memory) {
  GameState *game_state = static_cast<GameState *>(
      ArenaAlloc(memory.perma_memory, sizeof(GameState), alignof(GameState)));
  memory.game_state = game_state;

  game_state->asset =
      LoadPng("assets/wizard-idle.png", memory.perma_memory).value();
  game_state->sound =
      LoadWav("assets/coin.wav", 2, 48000, memory.perma_memory).value();
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

  if (input.keys[Input::Keys::Space].is_down) {
    game_state->sound_cursor = 0;
  }

  // -- Mix audio into the sound buffer --
  const size_t channels = sound_buffer.channels;
  const size_t frames_needed = sound_buffer.buffer.size() / channels;
  const size_t frames_left =
      game_state->sound.frame_count - game_state->sound_cursor;
  const size_t frames_to_copy = std::min(frames_left, frames_needed);

  if (frames_to_copy > 0) {
    const float *source =
        game_state->sound.data + game_state->sound_cursor * channels;
    float *dest = sound_buffer.buffer.data();

    for (size_t i = 0; i < frames_to_copy * channels; ++i) {
      dest[i] += source[i];
    }

    game_state->sound_cursor += frames_to_copy;
  }
};
