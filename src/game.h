#ifndef ALCHEMO_GAME
#define ALCHEMO_GAME

#include "arena.h"
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

typedef struct {
  int half_transition_count;
  bool is_down;
} GameKeyState;

typedef struct {
  int mouse_x;
  int mouse_y;
  int mouse_dx;
  int mouse_dy;
  GameKeyState keys[KEY_COUNT];
} GameInput;

typedef struct {
  Arena perma_memory;
  Arena temp_memory;
  void *game_state;
} GameMemory;

void GameInit(GameMemory *memory);
void GameUpdate(const GameInput *input, const float dt, GameMemory *memory,
                RenderCommands *render_commands);

#endif
