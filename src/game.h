#ifndef GAME_H
#define GAME_H

#include "mem.h"
#include "render.h"
#include <stdbool.h>
#include <stdint.h>

enum GameKeys {
  KEY_W = 1,
  KEY_A,
  KEY_S,
  KEY_D,
  KEY_SPACE,
  KEY_ENTER,
  KEY_ESC,
  KEY_MOUSE1,
  KEY_MOUSE2,

  KEY_COUNT,
};

typedef struct game_key_state {
  int half_transition_count;
  bool is_down;
} game_key_state;

typedef struct game_input {
  int mouse_x;
  int mouse_y;
  int mouse_dx;
  int mouse_dy;
  game_key_state keys[KEY_COUNT];
} game_input;

typedef struct game_memory {
  void *perma_memory;
  arena temp_memory;
  free_list free_list;
} game_memory;

void game_init(game_memory *memory);
void game_update(const game_input *input, const float dt, game_memory *memory,
                 render_commands *render_commands);

#endif
