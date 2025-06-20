#ifndef MEM_H
#define MEM_H

#include <stdint.h>

typedef struct arena {
  uintptr_t cursor;
  uintptr_t size;
  void *ptr;
} arena;
arena arena_init(void *buffer, uintptr_t size);
void *arena_alloc(arena *arena, uintptr_t size, uintptr_t alignment);
void arena_clear(arena *arena);
void arena_free(arena *arena);

typedef struct free_list_alloc_header {
  uintptr_t block_size;
  uintptr_t padding;
} free_list_alloc_header;

typedef struct free_list_node {
  struct free_list_node *next;
  uintptr_t block_size;
} free_list_node;

typedef struct free_list {
  void *data;
  uintptr_t size;
  uintptr_t used;

  free_list_node *head;
} free_list;

free_list free_list_init(void *buffer, uintptr_t size);
void *free_list_alloc(free_list *fl, uintptr_t size, uintptr_t alignment);
void free_list_dealloc(free_list *fl, void *data);
void free_list_free(free_list *fl);

#endif
