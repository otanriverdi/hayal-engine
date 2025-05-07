#ifndef HAYAL_MEM
#define HAYAL_MEM

#include <stddef.h>

namespace Mem {

struct FixedBuffer {
  bool initialized = false;
  size_t size;
  void *ptr;
};

template <typename T> T *FixedBufferCast(FixedBuffer &buffer);

struct Arena {
  size_t cursor = 0;
  size_t size;
  void *ptr;
};

template <typename T> T *ArenaAllocate(Arena &arena);

void ArenaClear(Arena &arena);

} // namespace Mem

#endif
