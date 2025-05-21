#ifndef HAYAL_MEM
#define HAYAL_MEM

#include <new>
#include <stddef.h>

namespace Mem {

struct Arena {
  size_t cursor = 0;
  size_t size;
  void *ptr;
};

inline void *ArenaAlloc(Arena &arena, size_t size) {
  if (arena.cursor + size > arena.size) {
    return nullptr;
  }
  void *start_of_block = static_cast<unsigned char *>(arena.ptr) + arena.cursor;
  arena.cursor += size;
  return start_of_block;
}

inline void ArenaReset(Arena &arena) { arena.cursor = 0; }

// STL compatible allocator
template <typename T> struct ArenaAllocator {
  using value_type = T;

  template <typename U> struct rebind {
    using other = ArenaAllocator<U>;
  };

  Arena &arena;

  explicit ArenaAllocator(Arena &a) noexcept : arena(a) {};

  T *allocate(size_t n) {
    void *mem = ArenaAlloc(arena, n * sizeof(T));
    if (!mem) {
      throw std::bad_alloc();
    }
    T *data = static_cast<T *>(ArenaAlloc(arena, n * sizeof(T)));
    if (data == nullptr) {
      throw std::bad_alloc();
    }

    return data;
  };

  void deallocate(T *ptr, size_t n) {
    // No-op
  }

  template <typename U> friend struct ArenaAllocator;
};

template <typename T, typename U>
bool operator==(const ArenaAllocator<T> &lhs,
                const ArenaAllocator<U> &rhs) noexcept {
  return lhs.arena == rhs.arena;
}

template <typename T, typename U>
bool operator!=(const ArenaAllocator<T> &lhs,
                const ArenaAllocator<U> &rhs) noexcept {
  return lhs.arena != rhs.arena;
}

} // namespace Mem

#endif
