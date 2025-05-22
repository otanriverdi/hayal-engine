#ifndef HAYAL_PLATFORM
#define HAYAL_PLATFORM

#include "mem.h"
#include <expected>
#include <filesystem>
#include <vector>

namespace hayal::platform {
enum class PlatformError { OPEN_FILE, READ_FILE };

std::expected<std::vector<char, ArenaAllocator<char>>, PlatformError>
ReadEntireFile(std::filesystem::path path, ArenaAllocator<char> &allocator);
} // namespace hayal::platform

#endif
