#include "asset.c"
#include "game.c"
#include "math.c"
#include "mem.c"
#include "mem.h"
#include "platform_linux.c"
#include "render.c"
#include "render_gl.c"
#include <SDL2/SDL.h>
#include <glad.h>
#include <stddef.h>

void parse_sdl_event(SDL_Window *window, SDL_Event *event, game_input *input,
                     renderer *renderer, bool *should_quit);

int main() {
  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "[PLATFORM]: %s", SDL_GetError());
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_Window *window =
      SDL_CreateWindow("hayal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       1920, 1080, SDL_WINDOW_OPENGL);
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
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                    "[PLATFORM]: Failed to load OpenGL context");
    return -1;
  }
  if (SDL_GL_SetSwapInterval(1) < 0) {
    SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "[PLATFORM] Unable to enable VSYNC: %s",
                SDL_GetError());
  }

  SDL_AudioSpec desired_spec = {
      .freq = 48000,
      .format = AUDIO_F32SYS,
      .channels = 2,
      .callback = NULL,
  };
  SDL_AudioSpec audio_spec;
  SDL_AudioDeviceID audio_device =
      SDL_OpenAudioDevice(NULL, 0, &desired_spec, &audio_spec, 0);
  if (audio_device == 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "[PLATFORM]: %s", SDL_GetError());
    return -1;
  }
  SDL_PauseAudioDevice(audio_device, 0);

  size_t perma_size = 1024 * 1024 * 1024;
  void *perma_buffer = malloc(perma_size);
  size_t temp_size = 250 * 1024 * 1024;
  void *temp_buffer = malloc(temp_size);
  size_t free_list_size = 1024 * 1024 * 1024;
  void *free_list_buffer = malloc(free_list_size);
  game_memory game_memory = {
      .perma_memory = perma_buffer,
      .temp_memory = arena_init(temp_buffer, temp_size),
      .free_list = free_list_init(free_list_buffer, free_list_size)};

  const uint64_t perf_frequency = SDL_GetPerformanceFrequency();
  uint64_t last_perf_counter = SDL_GetPerformanceCounter();

  game_init(&game_memory);

  renderer renderer = renderer_init(1920, 1080);
  render_commands render_commands = render_commands_init(2 * 1024 * 1024);

  bool should_quit = false;
  game_input input = {0};
  while (!should_quit) {
    uint64_t start_counter = SDL_GetPerformanceCounter();
    double dt =
        (double)(start_counter - last_perf_counter) / (double)perf_frequency;
    last_perf_counter = start_counter;

    input.mouse_dx = 0;
    input.mouse_dy = 0;
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      parse_sdl_event(window, &event, &input, &renderer, &should_quit);
    }

    game_update(&input, dt, &game_memory, &render_commands);
    renderer_process_commands(&renderer, &render_commands);
    SDL_GL_SwapWindow(window);

    render_commands_clear(&render_commands);
    arena_clear(&game_memory.temp_memory);
  }

  arena_free(&game_memory.temp_memory);
  free(perma_buffer);
  render_commands_free(&render_commands);
  SDL_CloseAudioDevice(audio_device);
  SDL_DestroyWindow(window);
  SDL_GL_DeleteContext(gl_context);
  SDL_Quit();

  return 0;
}

void parse_sdl_event(SDL_Window *window, SDL_Event *event, game_input *input,
                     renderer *renderer, bool *should_quit) {
  switch (event->type) {
  case SDL_QUIT:
    *should_quit = true;
    break;

  case SDL_WINDOWEVENT:
    if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
        event->window.event == SDL_WINDOWEVENT_RESIZED) {
      SDL_GL_GetDrawableSize(window, (int *)&renderer->framebuffer_size.x,
                             (int *)&renderer->framebuffer_size.y);
      glViewport(0, 0, (int)renderer->framebuffer_size.x,
                 (int)renderer->framebuffer_size.y);
      // }
      break;

    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      if (event->key.repeat) break;
      bool isDown = (event->type == SDL_KEYDOWN);
      switch (event->key.keysym.sym) {
      case SDLK_w: {
        game_key_state *k = &input->keys[KEY_W];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      } break;
      case SDLK_a: {
        game_key_state *k = &input->keys[KEY_A];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      } break;
      case SDLK_s: {
        game_key_state *k = &input->keys[KEY_S];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      } break;
      case SDLK_d: {
        game_key_state *k = &input->keys[KEY_D];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      } break;
      case SDLK_SPACE: {
        game_key_state *k = &input->keys[KEY_SPACE];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      } break;
      case SDLK_RETURN: {
        game_key_state *k = &input->keys[KEY_ENTER];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      } break;
      case SDLK_ESCAPE: {
        game_key_state *k = &input->keys[KEY_ESC];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      } break;
      default:
        break;
      }
    } break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      bool isDown = (event->type == SDL_MOUSEBUTTONDOWN);
      switch (event->button.button) {
      case SDL_BUTTON_LEFT: {
        game_key_state *k = &input->keys[KEY_MOUSE1];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      } break;
      case SDL_BUTTON_RIGHT: {
        game_key_state *k = &input->keys[KEY_MOUSE2];
        if (k->is_down != isDown) {
          k->is_down = isDown;
          k->half_transition_count++;
        }
      } break;

      default:
        break;
      }
    } break;

    case SDL_MOUSEMOTION: {
      input->mouse_x = event->motion.x;
      input->mouse_y = event->motion.y;
      input->mouse_dx += event->motion.xrel;
      input->mouse_dy += event->motion.yrel;
    } break;
    }
  }
}
