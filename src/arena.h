#ifndef ALCHEMO_ARENA
#define ALCHEMO_ARENA

#include <stdint.h>

typedef struct {
  uintptr_t cursor;
  uintptr_t size;
  void *ptr;
} Arena;

Arena ArenaInit(uintptr_t size);
void *ArenaAlloc(Arena *arena, uintptr_t size, uintptr_t alignement);
void ArenaClear(Arena *arena);
void ArenaFree(Arena *arena);

#endif
