#ifndef HAYAL_GAME
#define HAYAL_GAME

#include "mem.h"
#include "renderer.h"
#include <array>
#include <span>

using namespace hayal;

namespace hayal {

struct Memory {
  Arena perma_memory;
  Arena temp_memory;
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
void Update(const Input &input, const float dt, RenderCommands &render_commands,
            SoundBuffer &sound_buffer, Memory &memory);

} // namespace hayal

#endif
