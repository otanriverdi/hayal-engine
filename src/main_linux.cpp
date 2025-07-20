#include "game.cpp"
#include "game/asset.cpp"
#include "game/text.cpp"
#include "mem.cpp"
#include "mem.hpp"
#include "platform_linux.cpp"
#include "renderer_gl.cpp"
#include <SDL2/SDL.h>
#include <signal.h>

void parse_sdl_event(SDL_Window *window, SDL_Event *event, game_input *input, renderer *renderer,
                     bool *should_quit);

static volatile bool sigterm_received = false;
static void sigterm_handler(int sig) { sigterm_received = true; }

int main() {
  signal(SIGTERM, sigterm_handler);
  signal(SIGINT, sigterm_handler);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "[PLATFORM]: %s", SDL_GetError());
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_Window *window = SDL_CreateWindow("hayal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080,
                                        SDL_WINDOW_OPENGL);
  if (!window) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "[PLATFORM]: %s", SDL_GetError());
    return -1;
  }
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (!gl_context) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "[PLATFORM]: %s", SDL_GetError());
    return -1;
  }
  if (gladLoadGLLoader(SDL_GL_GetProcAddress) != 1) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[PLATFORM]: Failed to load OpenGL context");
    return -1;
  }
  if (SDL_GL_SetSwapInterval(1) < 0) {
    SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "[PLATFORM] Unable to enable VSYNC: %s", SDL_GetError());
  }

  game_memory game_memory = {.game_state = malloc(1 * GB),
                             .temp_allocator = arena_init(250 * MB),
                             .allocator = free_list_init(250 * MB)};
  assert(game_memory.game_state != NULL);

  const uint64_t perf_frequency = SDL_GetPerformanceFrequency();
  uint64_t last_perf_counter = SDL_GetPerformanceCounter();

  renderer renderer = renderer_init(1920, 1080);

  game_init(&game_memory, &renderer);

  bool should_quit = false;
  game_input input = {0};
  while (!should_quit && !sigterm_received) {
    uint64_t start_counter = SDL_GetPerformanceCounter();
    double dt = (double)(start_counter - last_perf_counter) / (double)perf_frequency;
    last_perf_counter = start_counter;

    game_reset_input(&input);
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      parse_sdl_event(window, &event, &input, &renderer, &should_quit);
    }

    game_update(&input, dt, &game_memory, &renderer);
    SDL_GL_SwapWindow(window);

    arena_clear(&game_memory.temp_allocator);
  }

  game_deinit(&game_memory, &renderer);
  renderer_destroy(&renderer);
  free_list_free(&game_memory.allocator);
  arena_free(&game_memory.temp_allocator);
  free(game_memory.game_state);

  SDL_DestroyWindow(window);
  SDL_GL_DeleteContext(gl_context);
  SDL_Quit();

  return 0;
}

typedef struct {
  SDL_Keycode sdl_key;
  int game_key;
} sdl_key_mapping;

static const sdl_key_mapping key_map[] = {
    // Movement keys
    {SDLK_w, KEY_W},
    {SDLK_a, KEY_A},
    {SDLK_s, KEY_S},
    {SDLK_d, KEY_D},

    // Arrow keys
    {SDLK_UP, KEY_UP},
    {SDLK_DOWN, KEY_DOWN},
    {SDLK_LEFT, KEY_LEFT},
    {SDLK_RIGHT, KEY_RIGHT},

    // Action keys
    {SDLK_SPACE, KEY_SPACE},
    {SDLK_RETURN, KEY_ENTER},
    {SDLK_ESCAPE, KEY_ESC},
    {SDLK_LSHIFT, KEY_LSHIFT},
    {SDLK_RSHIFT, KEY_RSHIFT},
    {SDLK_LCTRL, KEY_LCTRL},
    {SDLK_RCTRL, KEY_RCTRL},
    {SDLK_LALT, KEY_LALT},
    {SDLK_RALT, KEY_RALT},
    {SDLK_TAB, KEY_TAB},
    {SDLK_BACKSPACE, KEY_BACKSPACE},
    {SDLK_DELETE, KEY_DELETE},

    // Number keys
    {SDLK_1, KEY_1},
    {SDLK_2, KEY_2},
    {SDLK_3, KEY_3},
    {SDLK_4, KEY_4},
    {SDLK_5, KEY_5},
    {SDLK_6, KEY_6},
    {SDLK_7, KEY_7},
    {SDLK_8, KEY_8},
    {SDLK_9, KEY_9},
    {SDLK_0, KEY_0},

    // Letter keys
    {SDLK_q, KEY_Q},
    {SDLK_e, KEY_E},
    {SDLK_r, KEY_R},
    {SDLK_t, KEY_T},
    {SDLK_y, KEY_Y},
    {SDLK_u, KEY_U},
    {SDLK_i, KEY_I},
    {SDLK_o, KEY_O},
    {SDLK_p, KEY_P},
    {SDLK_f, KEY_F},
    {SDLK_g, KEY_G},
    {SDLK_h, KEY_H},
    {SDLK_j, KEY_J},
    {SDLK_k, KEY_K},
    {SDLK_l, KEY_L},
    {SDLK_z, KEY_Z},
    {SDLK_x, KEY_X},
    {SDLK_c, KEY_C},
    {SDLK_v, KEY_V},
    {SDLK_b, KEY_B},
    {SDLK_n, KEY_N},
    {SDLK_m, KEY_M},

    // Function keys
    {SDLK_F1, KEY_F1},
    {SDLK_F2, KEY_F2},
    {SDLK_F3, KEY_F3},
    {SDLK_F4, KEY_F4},
    {SDLK_F5, KEY_F5},
    {SDLK_F6, KEY_F6},
    {SDLK_F7, KEY_F7},
    {SDLK_F8, KEY_F8},
    {SDLK_F9, KEY_F9},
    {SDLK_F10, KEY_F10},
    {SDLK_F11, KEY_F11},
    {SDLK_F12, KEY_F12},

    // Keypad keys
    {SDLK_KP_0, KEY_KP_0},
    {SDLK_KP_1, KEY_KP_1},
    {SDLK_KP_2, KEY_KP_2},
    {SDLK_KP_3, KEY_KP_3},
    {SDLK_KP_4, KEY_KP_4},
    {SDLK_KP_5, KEY_KP_5},
    {SDLK_KP_6, KEY_KP_6},
    {SDLK_KP_7, KEY_KP_7},
    {SDLK_KP_8, KEY_KP_8},
    {SDLK_KP_9, KEY_KP_9},
    {SDLK_KP_ENTER, KEY_KP_ENTER},
    {SDLK_KP_PLUS, KEY_KP_PLUS},
    {SDLK_KP_MINUS, KEY_KP_MINUS},
    {SDLK_KP_MULTIPLY, KEY_KP_MULTIPLY},
    {SDLK_KP_DIVIDE, KEY_KP_DIVIDE},
    {SDLK_KP_PERIOD, KEY_KP_DECIMAL}};

static const int key_map_count = sizeof(key_map) / sizeof(key_map[0]);

static int sdl_key_to_game_key(SDL_Keycode sdl_key) {
  for (int i = 0; i < key_map_count; i++) {
    if (key_map[i].sdl_key == sdl_key) {
      return key_map[i].game_key;
    }
  }
  return KEY_UNKNOWN;
}

void parse_sdl_event(SDL_Window *window, SDL_Event *event, game_input *input, renderer *renderer,
                     bool *should_quit) {
  switch (event->type) {
  case SDL_QUIT:
    *should_quit = true;
    break;

  case SDL_WINDOWEVENT:
    if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
        event->window.event == SDL_WINDOWEVENT_RESIZED) {
      SDL_GL_GetDrawableSize(window, (int *)&renderer->framebuffer_size.x,
                             (int *)&renderer->framebuffer_size.y);
      glViewport(0, 0, (int)renderer->framebuffer_size.x, (int)renderer->framebuffer_size.y);
      break;

    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      if (event->key.repeat)
        break;
      bool isDown = (event->type == SDL_KEYDOWN);
      int key_index = sdl_key_to_game_key(event->key.keysym.sym);

      if (key_index != KEY_UNKNOWN) {
        game_key_state *k = &input->keys[key_index];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      }
    } break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      bool isDown = (event->type == SDL_MOUSEBUTTONDOWN);
      int key_index = KEY_UNKNOWN;

      switch (event->button.button) {
      case SDL_BUTTON_LEFT:
        key_index = KEY_MOUSE1;
        break;
      case SDL_BUTTON_RIGHT:
        key_index = KEY_MOUSE2;
        break;
      case SDL_BUTTON_MIDDLE:
        key_index = KEY_MOUSE3;
        break;
      case SDL_BUTTON_X1:
        key_index = KEY_MOUSE4;
        break;
      case SDL_BUTTON_X2:
        key_index = KEY_MOUSE5;
        break;
      default:
        break;
      }

      if (key_index != KEY_UNKNOWN) {
        game_key_state *k = &input->keys[key_index];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      }
    } break;

    case SDL_MOUSEMOTION: {
      input->mouse_x = event->motion.x;
      input->mouse_y = event->motion.y;
      input->mouse_dx += event->motion.xrel;
      input->mouse_dy += event->motion.yrel;
    } break;

    case SDL_MOUSEWHEEL: {
      input->mouse_wheel_x = event->wheel.x;
      input->mouse_wheel_y = event->wheel.y;
    } break;
    }
  }
}
