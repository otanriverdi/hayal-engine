#include "game.cpp"
#include "hayal/asset.cpp"
#include "hayal/mem.cpp"
#include "hayal/platform_linux.cpp"
#include "hayal/renderer.h"
#include "hayal/renderer_gl.cpp"
#include "hayal/sdl_wrapper.cpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <glad.h>
#include <span>

using namespace hayal;

static const int AUDIO_FREQ = 48000;
static const int AUDIO_CHANNELS = 2;
static const int WINDOW_WIDTH = 1920;
static const int WINDOW_HEIGHT = 1080;
static const char *WINDOW_TITLE = "hayal";
static constexpr std::size_t PERMA_STORAGE_SIZE =
    static_cast<std::size_t>(2) * 1024 * 1024 * 1024;
static constexpr std::size_t TEMP_STORAGE_SIZE =
    static_cast<std::size_t>(250) * 1024 * 1024;

Arena AllocateArena(size_t size) {
  Arena arena = {};
  arena.size = size;
  arena.ptr = malloc(size);
  return arena;
};

std::span<float> PrepareFrameAudioBuffer(hayal_SDL::Audio &audio) {
  const uint32_t queued_sample_bytes = SDL_GetQueuedAudioSize(audio.device);
  const size_t bytes_needed =
      queued_sample_bytes < audio.min_queued_bytes
          ? audio.target_queued_bytes - queued_sample_bytes
          : 0;
  const size_t samples_needed = bytes_needed / sizeof(float);
  size_t clamped_samples = std::min(samples_needed, audio.sample_buffer.size());
  std::span<float> sound_buffer(audio.sample_buffer.begin(), clamped_samples);
  std::fill(sound_buffer.begin(), sound_buffer.end(), 0.0f);
  return sound_buffer;
};

float CalculateDeltaTime(uint64_t perf_frequency, uint64_t &last_counter) {
  uint64_t start_counter = SDL_GetPerformanceCounter();
  float delta_time =
      static_cast<float>(start_counter - last_counter) / perf_frequency;
  last_counter = start_counter;
  return delta_time;
};

int main() {
  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
    hayal_SDL::LogCurrentError();
    return -1;
  }
  hayal_SDL::Window window;
  if (hayal_SDL::InitGlWindow(window, WINDOW_TITLE, WINDOW_WIDTH,
                              WINDOW_HEIGHT) != 0) {
    return -1;
  }
  hayal_SDL::Audio audio;
  if (hayal_SDL::InitAudio(audio, AUDIO_FREQ, AUDIO_CHANNELS) != 0) {
    return -1;
  }

  Renderer renderer =
      RendererInit(window.framebuffer_width, window.framebuffer_height);
  RenderCommands render_commands = {};

  Memory game_memory = {};
  game_memory.perma_memory = AllocateArena(PERMA_STORAGE_SIZE);
  game_memory.temp_memory = AllocateArena(TEMP_STORAGE_SIZE);

  SoundBuffer sound_buffer = {};
  sound_buffer.channels = audio.spec.channels;
  sound_buffer.freq = audio.spec.freq;

  const uint64_t perf_frequency = SDL_GetPerformanceFrequency();
  uint64_t last_perf_counter = SDL_GetPerformanceCounter();
  bool should_quit = false;

  Init(game_memory);

  while (!should_quit) {
    float dt = CalculateDeltaTime(perf_frequency, last_perf_counter);
    sound_buffer.buffer = PrepareFrameAudioBuffer(audio);
    Input input = {};

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      hayal_SDL::ParseEvent(event, input, window, should_quit);
    }

    Update(input, dt, render_commands, sound_buffer, game_memory);

    if (SDL_QueueAudio(audio.device, sound_buffer.buffer.data(),
                       sound_buffer.buffer.size_bytes()) < 0) {
      hayal_SDL::LogCurrentError();
    }

    CommandsRender(renderer, render_commands);

    SDL_GL_SwapWindow(window.handle);

    CommandsClear(render_commands);
    ArenaClear(game_memory.temp_memory);
  }

  ArenaFree(game_memory.perma_memory);
  ArenaFree(game_memory.temp_memory);
  RendererDestroy(renderer);
  SDL_GL_DeleteContext(window.gl_context);
  SDL_CloseAudioDevice(audio.device);
  SDL_DestroyWindow(window.handle);
  SDL_Quit();
};
