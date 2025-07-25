#ifndef MEM_H
#define MEM_H

#include <cstdint>
#include <stdint.h>

#define KB (1024UL)
#define MB (1024UL * KB)
#define GB (1024UL * MB)

struct arena {
  uintptr_t cursor;
  uintptr_t size;
  void *ptr;
};

struct free_list_alloc_header {
  uintptr_t block_size;
  uintptr_t padding;
};

struct free_list_node {
  struct free_list_node *next;
  uintptr_t block_size;
};

struct free_list {
  void *data;
  uintptr_t size;
  uintptr_t used;
  free_list_node *head;
};

enum allocator_type {
  ALLOCATOR_TYPE_FREE_LIST,
  ALLOCATOR_TYPE_ARENA,
};

struct mem_allocator {
  allocator_type type;
  union {
    arena arena;
    free_list free_list;
  };
};

mem_allocator allocator_arena_init(uintptr_t size);
mem_allocator allocator_free_list_init(uintptr_t size);
void allocator_destroy(mem_allocator *allocator);
void *allocator_alloc_impl(mem_allocator *allocator, uintptr_t size, uintptr_t alignment);
void allocator_dealloc(mem_allocator *allocator, void *data);
void allocator_clear(mem_allocator *allocator);

#define allocator_alloc(allocator, type, count)                                                              \
  (type *)allocator_alloc_impl((allocator), sizeof(type) * (count), alignof(type))

#endif
