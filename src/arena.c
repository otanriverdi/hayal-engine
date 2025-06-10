#include "arena.h"
#include <stdint.h>
#include <stdlib.h>

Arena ArenaInit(uintptr_t size) {
  Arena arena = {};
  arena.size = size;
  arena.ptr = malloc(size);
  return arena;
}

void *ArenaAlloc(Arena *arena, uintptr_t size, uintptr_t alignment) {
  uintptr_t base = (uintptr_t)(arena->ptr);
  uintptr_t current = base + arena->cursor;

  uintptr_t misalignment = current % alignment;
  if (misalignment != 0) {
    arena->cursor += alignment - misalignment;
  }

  if (arena->cursor + size > arena->size) {
    return NULL;
  }

  void *start_of_block = (unsigned char *)(arena->ptr) + arena->cursor;
  arena->cursor += size;
  return start_of_block;
}

void ArenaClear(Arena *arena) { arena->cursor = 0; }

void ArenaFree(Arena *arena) { free(arena->ptr); };
