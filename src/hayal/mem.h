#ifndef HAYAL_ARENA
#define HAYAL_ARENA

#include <cstddef>
#include <cstdint>
#include <new>

namespace hayal {

struct Arena {
  uintptr_t cursor = 0;
  uintptr_t size;
  void *ptr;
};

void *ArenaAlloc(Arena &arena, uintptr_t size, uintptr_t alignement);

void ArenaClear(Arena &arena);

void ArenaFree(Arena &arena);

template <typename T> struct ArenaAllocator {
  using value_type = T;

  template <typename U> struct rebind {
    using other = ArenaAllocator<U>;
  };

  Arena &arena;

  explicit ArenaAllocator(Arena &a) noexcept : arena(a) {};
  template <typename U>
  ArenaAllocator(const ArenaAllocator<U> &other) noexcept
      : arena(other.arena) {}

  T *allocate(size_t n) {
    void *mem = ArenaAlloc(arena, n * sizeof(T), alignof(T));
    if (!mem) {
      throw std::bad_alloc();
    }
    return static_cast<T *>(mem);
  };

  void deallocate(T *ptr, size_t n) {
    // No-op
  }

  template <typename U> friend struct ArenaAllocator;
};

template <typename T, typename U>
bool operator==(const ArenaAllocator<T> &lhs,
                const ArenaAllocator<U> &rhs) noexcept {
  return &lhs.arena == &rhs.arena;
}

template <typename T, typename U>
bool operator!=(const ArenaAllocator<T> &lhs,
                const ArenaAllocator<U> &rhs) noexcept {
  return !(lhs == rhs);
}

} // namespace hayal

#endif
