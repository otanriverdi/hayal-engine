#ifndef GAME_H
#define GAME_H

#include "game/asset.hpp"
#include "mem.hpp"
#include "renderer.hpp"
#include <stdbool.h>
#include <stdint.h>

enum GameKeys {
  KEY_UNKNOWN = 0,

  // Movement keys
  KEY_W = 1,
  KEY_A,
  KEY_S,
  KEY_D,

  // Arrow keys
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,

  // Action keys
  KEY_SPACE,
  KEY_ENTER,
  KEY_ESC,
  KEY_LSHIFT,
  KEY_RSHIFT,
  KEY_LCTRL,
  KEY_RCTRL,
  KEY_LALT,
  KEY_RALT,
  KEY_TAB,
  KEY_BACKSPACE,
  KEY_DELETE,

  // Number keys
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_0,

  // Letter keys
  KEY_Q,
  KEY_E,
  KEY_R,
  KEY_T,
  KEY_Y,
  KEY_U,
  KEY_I,
  KEY_O,
  KEY_P,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_Z,
  KEY_X,
  KEY_C,
  KEY_V,
  KEY_B,
  KEY_N,
  KEY_M,

  // Function keys
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12,

  // Keypad keys
  KEY_KP_0,
  KEY_KP_1,
  KEY_KP_2,
  KEY_KP_3,
  KEY_KP_4,
  KEY_KP_5,
  KEY_KP_6,
  KEY_KP_7,
  KEY_KP_8,
  KEY_KP_9,
  KEY_KP_ENTER,
  KEY_KP_PLUS,
  KEY_KP_MINUS,
  KEY_KP_MULTIPLY,
  KEY_KP_DIVIDE,
  KEY_KP_DECIMAL,

  // Mouse buttons
  KEY_MOUSE1,
  KEY_MOUSE2,
  KEY_MOUSE3,
  KEY_MOUSE4,
  KEY_MOUSE5,

  KEY_COUNT,
};

struct game_key_state {
  int half_transition_count;
  bool is_down;
};

struct game_input {
  int mouse_x;
  int mouse_y;
  int mouse_dx;
  int mouse_dy;
  int mouse_wheel_x;
  int mouse_wheel_y;
  game_key_state keys[KEY_COUNT];
};

struct game_memory {
  void *game_state;
  arena temp_allocator;
  free_list allocator;
};

static inline void game_reset_input(game_input *input) {
  input->mouse_dx = 0;
  input->mouse_dy = 0;
  input->mouse_wheel_x = 0;
  input->mouse_wheel_y = 0;

  for (int i = 0; i < KEY_COUNT; i++) {
    input->keys[i].half_transition_count = 0;
  }
}

void game_init(game_memory *memory, renderer *renderer, ma_engine *audio_player);
void game_update(const game_input *input, const float dt, game_memory *memory, struct renderer *renderer,
                 ma_engine *audio_player);
void game_deinit(game_memory *memory, struct renderer *renderer);

#endif
