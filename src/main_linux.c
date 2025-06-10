#include "arena.c"
#include "game.c"
#include <SDL2/SDL.h>
#include <glad.h>

void ParseSDLEvent(SDL_Event *event, GameInput *input, bool *should_quit);

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
      SDL_CreateWindow("alchemo", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_OPENGL);
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

  GameMemory game_memory = {.perma_memory = ArenaInit(1024 * 1024 * 1024),
                            .temp_memory = ArenaInit(250 * 1024 * 1024),
                            .game_state = NULL};

  const uint64_t perf_frequency = SDL_GetPerformanceFrequency();
  uint64_t last_perf_counter = SDL_GetPerformanceCounter();

  GameInit(&game_memory);

  bool should_quit = false;
  while (!should_quit) {
    uint64_t start_counter = SDL_GetPerformanceCounter();
    double dt =
        (double)(start_counter - last_perf_counter) / (double)perf_frequency;
    last_perf_counter = start_counter;

    GameInput input = {0};
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      ParseSDLEvent(&event, &input, &should_quit);
    }

    GameUpdate(&input, dt, &game_memory);

    SDL_GL_SwapWindow(window);
  }

  ArenaFree(&game_memory.temp_memory);
  ArenaFree(&game_memory.perma_memory);
  SDL_CloseAudioDevice(audio_device);
  SDL_DestroyWindow(window);
  SDL_GL_DeleteContext(gl_context);
  SDL_Quit();

  return 0;
}

void ParseSDLEvent(SDL_Event *event, GameInput *input, bool *should_quit) {
  switch (event->type) {
  case SDL_QUIT:
    *should_quit = true;
    break;

  case SDL_WINDOWEVENT:
  // if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
  //     event.window.event == SDL_WINDOWEVENT_RESIZED) {
  //   SDL_GL_GetDrawableSize(window.handle, &window.framebuffer_width,
  //                          &window.framebuffer_height);
  //   glViewport(0, 0, window.framebuffer_width, window.framebuffer_height);
  //   // }
  //   break;
  //
  case SDL_KEYDOWN:
  case SDL_KEYUP: {
    bool isDown = (event->type == SDL_KEYDOWN);
    switch (event->key.keysym.sym) {
    case SDLK_w: {
      GameKeyState *k = &input->keys[KEY_W];
      if (k->is_down != isDown) {
        k->is_down = isDown;
        k->half_transition_count++;
      }
    } break;
    case SDLK_a: {
      GameKeyState *k = &input->keys[KEY_A];
      if (k->is_down != isDown) {
        k->is_down = isDown;
        k->half_transition_count++;
      }
    } break;
    case SDLK_s: {
      GameKeyState *k = &input->keys[KEY_S];
      if (k->is_down != isDown) {
        k->is_down = isDown;
        k->half_transition_count++;
      }
    } break;
    case SDLK_d: {
      GameKeyState *k = &input->keys[KEY_D];
      if (k->is_down != isDown) {
        k->is_down = isDown;
        k->half_transition_count++;
      }
    } break;
    case SDLK_SPACE: {
      GameKeyState *k = &input->keys[KEY_SPACE];
      if (k->is_down != isDown) {
        k->is_down = isDown;
        k->half_transition_count++;
      }
    } break;
    case SDLK_RETURN: {
      GameKeyState *k = &input->keys[KEY_ENTER];
      if (k->is_down != isDown) {
        k->is_down = isDown;
        k->half_transition_count++;
      }
    } break;
    case SDLK_ESCAPE: {
      GameKeyState *k = &input->keys[KEY_ESC];
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
      GameKeyState k = input->keys[KEY_MOUSE1];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDL_BUTTON_RIGHT: {
      GameKeyState k = input->keys[KEY_MOUSE2];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
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
