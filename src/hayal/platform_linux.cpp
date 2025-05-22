#include "mem.h"
#include "platform.h"
#include <expected>
#include <fstream>
#include <vector>

using namespace hayal::platform;

std::expected<std::vector<char, hayal::ArenaAllocator<char>>, PlatformError>
hayal::platform::ReadEntireFile(std::filesystem::path path,
                                ArenaAllocator<char> &allocator) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file) {
    return std::unexpected(PlatformError::OPEN_FILE);
  }
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char, ArenaAllocator<char>> buffer(size, allocator);
  if (!file.read(buffer.data(), size)) {
    return std::unexpected(PlatformError::READ_FILE);
  }

  return buffer;
}
