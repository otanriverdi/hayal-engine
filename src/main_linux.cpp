#include "game.cpp"
#include "mem.cpp"
#include "renderer.hpp"
#include "renderer_gl.cpp"
#include "sdl_wrapper.cpp"
#include <SDL2/SDL.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <glad.h>
#include <span>
#include <sys/types.h>

static const int AUDIO_FREQ = 48000;
static const int AUDIO_CHANNELS = 2;
static const size_t MAX_SAMPLE_BUFFER_SIZE =
    (AUDIO_FREQ * 100 / 1000) * AUDIO_CHANNELS;
static const int WINDOW_WIDTH = 1920;
static const int WINDOW_HEIGHT = 1080;
static const char *WINDOW_TITLE = "hayal";
static constexpr std::size_t PERMA_STORAGE_SIZE =
    static_cast<std::size_t>(2) * 1024 * 1024 * 1024;
static constexpr std::size_t TEMP_STORAGE_SIZE =
    static_cast<std::size_t>(250) * 1024 * 1024;

Mem::FixedBuffer AllocateFixedBuffer() {
  Mem::FixedBuffer fixed_buffer = {};
  fixed_buffer.size = PERMA_STORAGE_SIZE;
  fixed_buffer.ptr = malloc(fixed_buffer.size);
  return fixed_buffer;
};

Mem::Arena AllocateArena() {
  Mem::Arena arena = {};
  arena.size = TEMP_STORAGE_SIZE;
  arena.ptr = malloc(TEMP_STORAGE_SIZE);
  return arena;
};

std::span<float> PrepareFrameAudioBuffer(SDL::Audio &audio) {
  const int queued_sample_bytes = SDL_GetQueuedAudioSize(audio.device);
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
    SDL::LogCurrentError();
    return -1;
  }
  SDL::Window window;
  if (SDL::InitGlWindow(window, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT) !=
      0) {
    return -1;
  }
  SDL::Audio audio;
  if (SDL::InitAudio(audio, AUDIO_FREQ, AUDIO_CHANNELS) != 0) {
    return -1;
  }

  Renderer::Renderer renderer = Renderer::RendererInit(
      window.framebuffer_width, window.framebuffer_height);
  Renderer::Commands render_commands = {};

  Game::Memory game_memory = {};
  game_memory.perma_memory = AllocateFixedBuffer();
  game_memory.temp_memory = AllocateArena();

  Game::SoundBuffer sound_buffer = {};
  sound_buffer.channels = audio.spec.channels;
  sound_buffer.freq = audio.spec.freq;

  const uint64_t perf_frequency = SDL_GetPerformanceFrequency();
  uint64_t last_perf_counter = SDL_GetPerformanceCounter();
  bool should_quit = false;

  Game::Init(game_memory);

  while (!should_quit) {
    float dt = CalculateDeltaTime(perf_frequency, last_perf_counter);
    sound_buffer.buffer = PrepareFrameAudioBuffer(audio);
    Game::Input input = {};

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      SDL::ParseEvent(event, input, window, should_quit);
    }

    Game::Update(input, dt, render_commands, sound_buffer, game_memory);

    if (SDL_QueueAudio(audio.device, sound_buffer.buffer.data(),
                       sound_buffer.buffer.size_bytes()) < 0) {
      SDL::LogCurrentError();
    }
    Renderer::CommandsRender(renderer, render_commands);
    SDL_GL_SwapWindow(window.handle);

    Renderer::CommandsClear(render_commands);
    Mem::ArenaClear(game_memory.temp_memory);
  }

  free(game_memory.perma_memory.ptr);
  free(game_memory.temp_memory.ptr);
  Renderer::RendererDestroy(renderer);
  SDL_GL_DeleteContext(window.gl_context);
  SDL_CloseAudioDevice(audio.device);
  SDL_DestroyWindow(window.handle);
  SDL_Quit();
};
