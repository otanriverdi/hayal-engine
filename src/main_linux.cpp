#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <array>
#include <cstdint>
#include <glad.h>
#include <span>

static const int AUDIO_FREQ = 48000;
static const int AUDIO_CHANNELS = 2;
static const size_t MAX_SAMPLE_BUFFER_SIZE =
    (AUDIO_FREQ * 100 / 1000) * AUDIO_CHANNELS;
static const int WINDOW_WIDTH = 1920;
static const int WINDOW_HEIGHT = 1080;
static const char *WINDOW_TITLE = "hayal";

void SDLLogCurrentError() {
  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "[PLATFORM]: %s", SDL_GetError());
}

struct Window {
  SDL_Window *handle;
  SDL_GLContext gl_context;
};
int SDLInitGlWindow(Window *out) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_Window *handle = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                        WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
  if (!handle) {
    SDLLogCurrentError();
    return -1;
  }
  SDL_GLContext gl_context = SDL_GL_CreateContext(handle);
  if (!gl_context) {
    SDLLogCurrentError();
    return -1;
  }
  if (gladLoadGLLoader(SDL_GL_GetProcAddress) != 1) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                    "[GLAD]: Failed to initialize GLAD");
    return -1;
  }
  if (SDL_GL_SetSwapInterval(1) < 0) {
    SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Unable to set VSYNC: %s",
                SDL_GetError());
  }
  *out = Window{
      handle,
      gl_context,
  };
  return 0;
};

struct Audio {
  size_t target_queued_bytes;
  size_t min_queued_bytes;
  SDL_AudioDeviceID device;
  SDL_AudioSpec spec;
  std::array<float, MAX_SAMPLE_BUFFER_SIZE> sample_buffer;
};
int SDLInitAudio(Audio *out) {
  SDL_AudioSpec desired_spec = {};
  desired_spec.freq = AUDIO_FREQ;
  desired_spec.format = AUDIO_F32SYS;
  desired_spec.channels = AUDIO_CHANNELS;
  desired_spec.callback = nullptr;
  SDL_AudioSpec spec;
  SDL_AudioDeviceID device =
      SDL_OpenAudioDevice(NULL, 0, &desired_spec, &spec, 0);
  if (device == 0) {
    SDLLogCurrentError();
    return -1;
  }
  SDL_PauseAudioDevice(device, 0);
  const size_t bytes_per_second = spec.freq * spec.channels * sizeof(float);
  const size_t target_queued_bytes = bytes_per_second * 50 / 1000;
  const size_t min_queued_bytes = bytes_per_second * 25 / 1000;
  *out = Audio{
      target_queued_bytes, min_queued_bytes, device, spec, {},
  };
  return 0;
};

std::span<float> PrepareFrameAudioBuffer(Audio *audio) {
  const int queued_sample_bytes = SDL_GetQueuedAudioSize(audio->device);
  const size_t bytes_needed =
      queued_sample_bytes < audio->min_queued_bytes
          ? audio->target_queued_bytes - queued_sample_bytes
          : 0;
  const size_t samples_needed = bytes_needed / sizeof(float);
  size_t clamped_samples =
      std::min(samples_needed, audio->sample_buffer.size());
  std::span<float> sound_buffer(audio->sample_buffer.begin(), clamped_samples);
  std::fill(sound_buffer.begin(), sound_buffer.end(), 0.0f);
  return sound_buffer;
};

float CalculateDeltaTime(uint64_t perf_frequency, uint64_t *last_counter) {
  uint64_t start_counter = SDL_GetPerformanceCounter();
  float delta_time =
      static_cast<float>(start_counter - *last_counter) / perf_frequency;
  *last_counter = start_counter;
  return delta_time;
};

int main() {
  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
    SDLLogCurrentError();
    return -1;
  }
  Window window;
  if (SDLInitGlWindow(&window) != 0) {
    return -1;
  }

  Audio audio;
  if (SDLInitAudio(&audio) != 0) {
    return -1;
  }

  const uint64_t perf_frequency = SDL_GetPerformanceFrequency();
  uint64_t last_perf_counter = SDL_GetPerformanceCounter();
  bool should_quit = false;
  while (!should_quit) {
    float delta_time = CalculateDeltaTime(perf_frequency, &last_perf_counter);

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
      case SDL_QUIT:
        should_quit = true;
        break;
      }
    }

    std::span<float> sound_buffer = PrepareFrameAudioBuffer(&audio);

    if (SDL_QueueAudio(audio.device, sound_buffer.data(),
                       sound_buffer.size_bytes()) < 0) {
      SDLLogCurrentError();
    }
    SDL_GL_SwapWindow(window.handle);
  }

  SDL_GL_DeleteContext(window.gl_context);
  SDL_CloseAudioDevice(audio.device);
  SDL_DestroyWindow(window.handle);
  SDL_Quit();
};
