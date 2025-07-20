#include "game.cpp"
#include "game/asset.cpp"
#include "game/text.cpp"
#include "mem.cpp"
#include "mem.h"
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
      int key_index = KEY_UNKNOWN;

      switch (event->key.keysym.sym) {
      // Movement keys
      case SDLK_w:
        key_index = KEY_W;
        break;
      case SDLK_a:
        key_index = KEY_A;
        break;
      case SDLK_s:
        key_index = KEY_S;
        break;
      case SDLK_d:
        key_index = KEY_D;
        break;

      // Arrow keys
      case SDLK_UP:
        key_index = KEY_UP;
        break;
      case SDLK_DOWN:
        key_index = KEY_DOWN;
        break;
      case SDLK_LEFT:
        key_index = KEY_LEFT;
        break;
      case SDLK_RIGHT:
        key_index = KEY_RIGHT;
        break;

      // Action keys
      case SDLK_SPACE:
        key_index = KEY_SPACE;
        break;
      case SDLK_RETURN:
        key_index = KEY_ENTER;
        break;
      case SDLK_ESCAPE:
        key_index = KEY_ESC;
        break;
      case SDLK_LSHIFT:
        key_index = KEY_LSHIFT;
        break;
      case SDLK_RSHIFT:
        key_index = KEY_RSHIFT;
        break;
      case SDLK_LCTRL:
        key_index = KEY_LCTRL;
        break;
      case SDLK_RCTRL:
        key_index = KEY_RCTRL;
        break;
      case SDLK_LALT:
        key_index = KEY_LALT;
        break;
      case SDLK_RALT:
        key_index = KEY_RALT;
        break;
      case SDLK_TAB:
        key_index = KEY_TAB;
        break;
      case SDLK_BACKSPACE:
        key_index = KEY_BACKSPACE;
        break;
      case SDLK_DELETE:
        key_index = KEY_DELETE;
        break;

      // Number keys
      case SDLK_1:
        key_index = KEY_1;
        break;
      case SDLK_2:
        key_index = KEY_2;
        break;
      case SDLK_3:
        key_index = KEY_3;
        break;
      case SDLK_4:
        key_index = KEY_4;
        break;
      case SDLK_5:
        key_index = KEY_5;
        break;
      case SDLK_6:
        key_index = KEY_6;
        break;
      case SDLK_7:
        key_index = KEY_7;
        break;
      case SDLK_8:
        key_index = KEY_8;
        break;
      case SDLK_9:
        key_index = KEY_9;
        break;
      case SDLK_0:
        key_index = KEY_0;
        break;

      // Letter keys
      case SDLK_q:
        key_index = KEY_Q;
        break;
      case SDLK_e:
        key_index = KEY_E;
        break;
      case SDLK_r:
        key_index = KEY_R;
        break;
      case SDLK_t:
        key_index = KEY_T;
        break;
      case SDLK_y:
        key_index = KEY_Y;
        break;
      case SDLK_u:
        key_index = KEY_U;
        break;
      case SDLK_i:
        key_index = KEY_I;
        break;
      case SDLK_o:
        key_index = KEY_O;
        break;
      case SDLK_p:
        key_index = KEY_P;
        break;
      case SDLK_f:
        key_index = KEY_F;
        break;
      case SDLK_g:
        key_index = KEY_G;
        break;
      case SDLK_h:
        key_index = KEY_H;
        break;
      case SDLK_j:
        key_index = KEY_J;
        break;
      case SDLK_k:
        key_index = KEY_K;
        break;
      case SDLK_l:
        key_index = KEY_L;
        break;
      case SDLK_z:
        key_index = KEY_Z;
        break;
      case SDLK_x:
        key_index = KEY_X;
        break;
      case SDLK_c:
        key_index = KEY_C;
        break;
      case SDLK_v:
        key_index = KEY_V;
        break;
      case SDLK_b:
        key_index = KEY_B;
        break;
      case SDLK_n:
        key_index = KEY_N;
        break;
      case SDLK_m:
        key_index = KEY_M;
        break;

      // Function keys
      case SDLK_F1:
        key_index = KEY_F1;
        break;
      case SDLK_F2:
        key_index = KEY_F2;
        break;
      case SDLK_F3:
        key_index = KEY_F3;
        break;
      case SDLK_F4:
        key_index = KEY_F4;
        break;
      case SDLK_F5:
        key_index = KEY_F5;
        break;
      case SDLK_F6:
        key_index = KEY_F6;
        break;
      case SDLK_F7:
        key_index = KEY_F7;
        break;
      case SDLK_F8:
        key_index = KEY_F8;
        break;
      case SDLK_F9:
        key_index = KEY_F9;
        break;
      case SDLK_F10:
        key_index = KEY_F10;
        break;
      case SDLK_F11:
        key_index = KEY_F11;
        break;
      case SDLK_F12:
        key_index = KEY_F12;
        break;

      // Keypad keys
      case SDLK_KP_0:
        key_index = KEY_KP_0;
        break;
      case SDLK_KP_1:
        key_index = KEY_KP_1;
        break;
      case SDLK_KP_2:
        key_index = KEY_KP_2;
        break;
      case SDLK_KP_3:
        key_index = KEY_KP_3;
        break;
      case SDLK_KP_4:
        key_index = KEY_KP_4;
        break;
      case SDLK_KP_5:
        key_index = KEY_KP_5;
        break;
      case SDLK_KP_6:
        key_index = KEY_KP_6;
        break;
      case SDLK_KP_7:
        key_index = KEY_KP_7;
        break;
      case SDLK_KP_8:
        key_index = KEY_KP_8;
        break;
      case SDLK_KP_9:
        key_index = KEY_KP_9;
        break;
      case SDLK_KP_ENTER:
        key_index = KEY_KP_ENTER;
        break;
      case SDLK_KP_PLUS:
        key_index = KEY_KP_PLUS;
        break;
      case SDLK_KP_MINUS:
        key_index = KEY_KP_MINUS;
        break;
      case SDLK_KP_MULTIPLY:
        key_index = KEY_KP_MULTIPLY;
        break;
      case SDLK_KP_DIVIDE:
        key_index = KEY_KP_DIVIDE;
        break;
      case SDLK_KP_PERIOD:
        key_index = KEY_KP_DECIMAL;
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
