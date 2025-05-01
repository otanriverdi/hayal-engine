#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <glad.h>

static const int WINDOW_WIDTH = 1920;
static const int WINDOW_HEIGHT = 1080;
static const char *WINDOW_TITLE = "hayal";

void SDLLogCurrentError() {
  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "[PLATFORM]: %s", SDL_GetError());
}

int main() {
  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
    SDLLogCurrentError();
    return -1;
  }
  SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                        WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
  if (window == NULL) {
    SDLLogCurrentError();
    return -1;
  }
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (gl_context == NULL) {
    SDLLogCurrentError();
    return -1;
  }
  if (gladLoadGLLoader(SDL_GL_GetProcAddress) != 1) {
    SDLLogCurrentError();
    return -1;
  }
  if (SDL_GL_SetSwapInterval(1) < 0) {
    SDLLogCurrentError();
    return -1;
  }

  SDL_AudioSpec audio_spec = {
      .freq = 48000,
      .format = AUDIO_F32,
      .channels = 2,
  };
  SDL_AudioDeviceID audio_device =
      SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
  if (!audio_device) {
    SDLLogCurrentError();
    return -1;
  }
  SDL_PauseAudioDevice(audio_device, 0);

  uint64_t perf_frequency = SDL_GetPerformanceFrequency();
  uint64_t last_perf_counter = SDL_GetPerformanceCounter();

  bool should_quit = false;
  while (!should_quit) {
    int start_perf_counter = SDL_GetPerformanceCounter();
    double delta_time = (double)(start_perf_counter - last_perf_counter) /
                        (double)perf_frequency;
    last_perf_counter = start_perf_counter;

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
      case SDL_QUIT:
        should_quit = true;
        break;
      }
    }
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(gl_context);
  SDL_CloseAudioDevice(audio_device);
  SDL_DestroyWindow(window);
  SDL_Quit();
};
