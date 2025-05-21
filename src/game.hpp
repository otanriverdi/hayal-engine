#ifndef HAYAL_GAME
#define HAYAL_GAME

#include "mem.hpp"
#include "renderer.hpp"
#include <array>
#include <span>

namespace Game {

struct Memory {
  Mem::Arena perma_memory;
  Mem::Arena temp_memory;
  void *game_state;
};

struct Input {
  enum Keys {
    W = 1,
    A,
    S,
    D,
    Space,
    Enter,
    Esc,
    Mouse1,
    Mouse2,

    KeyCount,
  };

  struct KeyState {
    int half_transition_count;
    bool is_down;
  };

  int mouse_x = 0;
  int mouse_y = 0;

  int mouse_dx = 0;
  int mouse_dy = 0;

  std::array<KeyState, Keys::KeyCount> keys = {};
};

struct SoundBuffer {
  std::span<float> buffer;
  int channels;
  int freq;
};

void Init(Memory &memory);
void Update(const Input &input, const float dt,
            Renderer::Commands &render_commands, SoundBuffer &sound_buffer,
            Memory &memory);

} // namespace Game

#endif
