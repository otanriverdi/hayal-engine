#include "mem.hpp"
#include <cassert>
#include <cstddef>

template <typename T> T *Mem::FixedBufferCast(FixedBuffer &buffer) {
  if (!buffer.initialized) {
    return NULL;
  }
  assert(sizeof(T) <= buffer.size);
  return reinterpret_cast<T *>(buffer.ptr);
}

template <typename T> T *Mem::ArenaAllocate(Arena &arena) {
  size_t size = sizeof(T);
  if (arena.cursor + size <= arena.size) {
    return nullptr;
  }
  void *start_of_block = static_cast<unsigned char *>(arena.ptr) + arena.cursor;
  arena.cursor += size;
  return static_cast<T *>(start_of_block);
}

void Mem::ArenaClear(Arena &arena) { arena.cursor = 0; }
