#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <array>
#include <glad.h>

using namespace hayal;

namespace hayal_SDL {

void LogCurrentError() {
  SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "[PLATFORM]: %s", SDL_GetError());
}

struct Window {
  SDL_Window *handle;
  SDL_GLContext gl_context;
  int framebuffer_width, framebuffer_height;
};
int InitGlWindow(Window &out, const char *title, int width, int height) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  out.handle =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, SDL_WINDOW_OPENGL);
  if (!out.handle) {
    LogCurrentError();
    return -1;
  }
  out.gl_context = SDL_GL_CreateContext(out.handle);
  if (!out.gl_context) {
    LogCurrentError();
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

static const size_t MAX_SAMPLE_BUFFER_SIZE = (48000 * 100 / 1000) * 2;
struct Audio {
  size_t target_queued_bytes;
  size_t min_queued_bytes;
  SDL_AudioDeviceID device;
  SDL_AudioSpec spec;
  std::array<float, MAX_SAMPLE_BUFFER_SIZE> sample_buffer;
};
int InitAudio(Audio &out, int freq, int channels) {
  SDL_AudioSpec desired_spec = {};
  desired_spec.freq = freq;
  desired_spec.format = AUDIO_F32SYS;
  desired_spec.channels = channels;
  desired_spec.callback = nullptr;
  out.device = SDL_OpenAudioDevice(NULL, 0, &desired_spec, &out.spec, 0);
  if (out.device == 0) {
    LogCurrentError();
    return -1;
  }
  SDL_PauseAudioDevice(out.device, 0);
  const size_t bytes_per_second =
      out.spec.freq * out.spec.channels * sizeof(float);
  out.target_queued_bytes = bytes_per_second * 50 / 1000;
  out.min_queued_bytes = bytes_per_second * 25 / 1000;
  return 0;
};

void ParseEvent(SDL_Event &event, Input &input, Window &window,
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
      auto &k = input.keys[Input::Keys::W];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_a: {
      auto &k = input.keys[Input::Keys::A];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_s: {
      auto &k = input.keys[Input::Keys::S];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_d: {
      auto &k = input.keys[Input::Keys::D];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_SPACE: {
      auto &k = input.keys[Input::Keys::Space];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_RETURN: {
      auto &k = input.keys[Input::Keys::Enter];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDLK_ESCAPE: {
      auto &k = input.keys[Input::Keys::Esc];
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
      auto &k = input.keys[Input::Keys::Mouse1];
      if (k.is_down != isDown) {
        k.is_down = isDown;
        k.half_transition_count++;
      }
    } break;
    case SDL_BUTTON_RIGHT: {
      auto &k = input.keys[Input::Keys::Mouse2];
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

}; // namespace hayal_SDL
