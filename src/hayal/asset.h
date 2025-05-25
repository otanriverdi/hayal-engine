#ifndef HAYAL_ASSETS
#define HAYAL_ASSETS

#include "glm/glm.hpp"
#include "mem.h"
#include <expected>

namespace hayal {

enum class AssetError { READ_FILE };

struct Image {
  glm::vec2 size;
  unsigned char *data;
  std::optional<uint32_t> texture_handle;
};

std::expected<Image, AssetError> LoadPng(std::string path, Arena arena);

} // namespace hayal

#endif
