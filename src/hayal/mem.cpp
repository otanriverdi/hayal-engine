#include "mem.h"
#include <cstdint>
#include <cstdlib>

void *hayal::ArenaAlloc(Arena &arena, uintptr_t size, uintptr_t alignment) {
  uintptr_t misalignment = arena.cursor % alignment;
  if (misalignment != 0) {
    arena.cursor += alignment - misalignment;
  }

  if (arena.cursor + size > arena.size) {
    return nullptr;
  }
  void *start_of_block = static_cast<unsigned char *>(arena.ptr) + arena.cursor;
  arena.cursor += size;
  return start_of_block;
}

void hayal::ArenaClear(Arena &arena) { arena.cursor = 0; }

void hayal::ArenaFree(Arena &arena) { free(arena.ptr); };
