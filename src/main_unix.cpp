#include "game.cpp"
#include "mem.cpp"
#include "renderer.hpp"
#include "renderer_gl.cpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <array>
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

void SDLLogCurrentError() {
  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "[PLATFORM]: %s", SDL_GetError());
}

struct Window {
  SDL_Window *handle;
  SDL_GLContext gl_context;
  int framebuffer_width, framebuffer_height;
};
int SDLInitGlWindow(Window &out) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  out.handle = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
  if (!out.handle) {
    SDLLogCurrentError();
    return -1;
  }
  out.gl_context = SDL_GL_CreateContext(out.handle);
  if (!out.gl_context) {
    SDLLogCurrentError();
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
  SDL_GL_GetDrawableSize(out.handle, &out.framebuffer_width,
                         &out.framebuffer_height);
  return 0;
};

struct Audio {
  size_t target_queued_bytes;
  size_t min_queued_bytes;
  SDL_AudioDeviceID device;
  SDL_AudioSpec spec;
  std::array<float, MAX_SAMPLE_BUFFER_SIZE> sample_buffer;
};
int SDLInitAudio(Audio &out) {
  SDL_AudioSpec desired_spec = {};
  desired_spec.freq = AUDIO_FREQ;
  desired_spec.format = AUDIO_F32SYS;
  desired_spec.channels = AUDIO_CHANNELS;
  desired_spec.callback = nullptr;
  SDL_AudioSpec spec;
  out.device = SDL_OpenAudioDevice(NULL, 0, &desired_spec, &out.spec, 0);
  if (out.device == 0) {
    SDLLogCurrentError();
    return -1;
  }
  SDL_PauseAudioDevice(out.device, 0);
  const size_t bytes_per_second = spec.freq * spec.channels * sizeof(float);
  out.target_queued_bytes = bytes_per_second * 50 / 1000;
  out.min_queued_bytes = bytes_per_second * 25 / 1000;
  return 0;
};

void SDLParseEvent(SDL_Event &event, Game::Input &input, Window &window,
                   bool &should_quit) {
  switch (event.type) {
  case SDL_QUIT:
    should_quit = true;
    break;

  case SDL_WINDOWEVENT:
    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
        event.window.event == SDL_WINDOWEVENT_RESIZED) {
      SDL_GL_GetDrawableSize(window.handle, &window.framebuffer_width,
                             &window.framebuffer_height);
      glViewport(0, 0, window.framebuffer_width, window.framebuffer_height);
    }
    break;

  case SDL_KEYDOWN:
  case SDL_KEYUP: {
    bool isDown = (event.type == SDL_KEYDOWN);
    switch (event.key.keysym.sym) {
    case SDLK_w: {
      auto &k = input.keys[Game::Input::Keys::W];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_a: {
      auto &k = input.keys[Game::Input::Keys::A];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_s: {
      auto &k = input.keys[Game::Input::Keys::S];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_d: {
      auto &k = input.keys[Game::Input::Keys::D];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_SPACE: {
      auto &k = input.keys[Game::Input::Keys::Space];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_RETURN: {
      auto &k = input.keys[Game::Input::Keys::Enter];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_ESCAPE: {
      auto &k = input.keys[Game::Input::Keys::Esc];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    default:
      break;
    }
  } break;

  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP: {
    bool isDown = (event.type == SDL_MOUSEBUTTONDOWN);
    switch (event.button.button) {
    case SDL_BUTTON_LEFT: {
      auto &k = input.keys[Game::Input::Keys::Mouse1];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDL_BUTTON_RIGHT: {
      auto &k = input.keys[Game::Input::Keys::Mouse2];
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
    input.mouse_x = event.motion.x;
    input.mouse_y = event.motion.y;
    input.mouse_dx += event.motion.xrel;
    input.mouse_dy += event.motion.yrel;
  } break;
  }
}

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

std::span<float> PrepareFrameAudioBuffer(Audio &audio) {
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
    SDLLogCurrentError();
    return -1;
  }
  Window window;
  if (SDLInitGlWindow(window) != 0) {
    return -1;
  }
  Audio audio;
  if (SDLInitAudio(audio) != 0) {
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
      SDLParseEvent(event, input, window, should_quit);
    }

    Game::Update(input, dt, render_commands, sound_buffer, game_memory);

    if (SDL_QueueAudio(audio.device, sound_buffer.buffer.data(),
                       sound_buffer.buffer.size_bytes()) < 0) {
      SDLLogCurrentError();
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
