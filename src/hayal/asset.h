#ifndef HAYAL_ASSETS
#define HAYAL_ASSETS

#include "glm/glm.hpp"
#include "mem.h"
#include <expected>

namespace hayal {

enum class AssetError { READ_FILE, DECODE_AUDIO, NO_ALLOC };

struct Image {
  glm::vec2 size;
  unsigned char *data;
  std::optional<uint32_t> texture_handle;
};

std::expected<Image, AssetError> LoadPng(std::string path, Arena arena);

struct Sound {
  float *data;
  uint64_t frame_count;
};

std::expected<Sound, AssetError> LoadWav(std::string path, int channels,
                                         int freq, Arena arena);

} // namespace hayal

#endif
