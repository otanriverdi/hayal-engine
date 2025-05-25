#include "asset.h"
#include "platform.h"
#include "stb_image.h"

using namespace hayal;

std::expected<Image, AssetError> hayal::LoadPng(std::string path, Arena arena) {
  auto allocator = ArenaAllocator<unsigned char>(arena);
  auto res = platform::ReadEntireFile(path, allocator);
  if (!res.has_value()) {
    return std::unexpected(AssetError::READ_FILE);
  };
  auto file = res.value();

  Image image = {};
  int x, y, n;
  image.data = stbi_load_from_memory(file.data(), static_cast<int>(file.size()),
                                     &x, &y, &n, 4);
  image.size.x = x;
  image.size.y = y;

  return image;
};
