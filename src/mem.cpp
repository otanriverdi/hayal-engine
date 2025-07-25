#include "mem.hpp"
#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>

static arena arena_init(uintptr_t size) {
  arena arena = {};
  arena.size = size;
  arena.ptr = malloc(size);
  assert(arena.ptr != NULL);
  return arena;
}

static void *arena_alloc(arena *arena, uintptr_t size, uintptr_t alignment) {
  uintptr_t base = (uintptr_t)(arena->ptr);
  uintptr_t current = base + arena->cursor;

  uintptr_t misalignment = current % alignment;
  if (misalignment != 0) {
    arena->cursor += alignment - misalignment;
  }
  assert(arena->cursor + size <= arena->size);
  void *start_of_block = (unsigned char *)(arena->ptr) + arena->cursor;
  arena->cursor += size;
  return start_of_block;
}

static void arena_clear(arena *arena) { arena->cursor = 0; }

static void arena_free(arena *arena) { free(arena->ptr); };

static free_list free_list_init(uintptr_t size) {
  free_list fl;

  fl.data = malloc(size);
  assert(fl.data != NULL);

  free_list_node *initial_node = (free_list_node *)fl.data;
  initial_node->block_size = size;
  initial_node->next = NULL;

  fl.head = initial_node;
  fl.used = 0;

  return fl;
}

static size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size);
static void free_list_node_insert(free_list_node **phead, free_list_node *prev_node,
                                  free_list_node *new_node);
static void free_list_node_remove(free_list_node **phead, free_list_node *prev_node,
                                  free_list_node *del_node);

static void *free_list_alloc(free_list *fl, uintptr_t size, uintptr_t alignment) {
  // we enforce a minimum size and alignment
  if (size < sizeof(free_list_node)) {
    size = sizeof(free_list_node);
  }
  if (alignment < 8) {
    alignment = 8;
  }

  // find the suitable node
  size_t padding = 0;
  free_list_node *node = fl->head;
  free_list_node *prev_node = NULL;
  while (node != NULL) {
    // determine how much more padding we need to fit the header
    padding = calc_padding_with_header((uintptr_t)node, alignment, sizeof(free_list_alloc_header));
    size_t required = size + padding;
    if (node->block_size >= required) {
      break;
    }
    prev_node = node;
    node = node->next;
  }

  assert(node != NULL);

  // padding before the header
  size_t alignment_padding = padding - sizeof(free_list_alloc_header);
  size_t required_space = size + padding;
  size_t remaining_space = node->block_size - required_space;

  // there is empty space in the node we can create a new block from
  if (remaining_space > 0) {
    free_list_node *new_node = (free_list_node *)((char *)node + required_space);
    new_node->block_size = remaining_space;
    free_list_node_insert(&fl->head, node, new_node);
  }

  free_list_node_remove(&fl->head, prev_node, node);

  // we insert the header to the location we reserved for it
  free_list_alloc_header *header_ptr = (free_list_alloc_header *)((char *)node + alignment_padding);
  header_ptr->block_size = required_space;
  header_ptr->padding = alignment_padding;

  // return the pointer after the header where the usable memory begins
  return (void *)((char *)header_ptr + sizeof(free_list_alloc_header));
}

static void free_list_dealloc(free_list *fl, void *ptr) {
  if (ptr == NULL) {
    return;
  }

  free_list_alloc_header *header = (free_list_alloc_header *)((char *)ptr - sizeof(free_list_alloc_header));
  free_list_node *free_node = (free_list_node *)header;
  free_node->block_size = header->block_size + header->padding;
  free_node->next = NULL;

  free_list_node *node = fl->head;
  free_list_node *prev_node = NULL;
  while (node != NULL) {
    if ((free_list_node *)ptr < node) {
      free_list_node_insert(&fl->head, prev_node, free_node);
      break;
    }
    prev_node = node;
    node = node->next;
  }

  fl->used = free_node->block_size;

  if (free_node->next != NULL && (void *)((char *)free_node + free_node->block_size) == free_node->next) {
    free_node->block_size += free_node->next->block_size;
    free_list_node_remove(&fl->head, free_node, free_node->next);
  }

  if (prev_node != NULL && prev_node->next != NULL &&
      (void *)((char *)prev_node + prev_node->block_size) == free_node) {
    prev_node->block_size += free_node->next->block_size;
    free_list_node_remove(&fl->head, prev_node, free_node);
  }
}

static void free_list_free(free_list *fl) {
  if (fl == NULL) {
    return;
  }

  fl->head = NULL;
  fl->used = 0;
  free(fl->data);
}

static size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size) {
  uintptr_t p, a, modulo, padding, needed_space;

  p = ptr;
  a = alignment;
  modulo = p & (a - 1);

  padding = 0;
  needed_space = 0;

  if (modulo != 0) {
    padding = a - modulo;
  }

  needed_space = (uintptr_t)header_size;

  if (padding < needed_space) {
    needed_space -= padding;

    if ((needed_space & (a - 1)) != 0) {
      padding += a * (1 + (needed_space / a));
    } else {
      padding += a * (needed_space / a);
    }
  }

  return (size_t)padding;
}
static void free_list_node_insert(free_list_node **phead, free_list_node *prev_node,
                                  free_list_node *new_node) {
  if (prev_node == NULL) {
    if (*phead != NULL) {
      new_node->next = *phead;
    } else {
      *phead = new_node;
    }
  } else {
    if (prev_node->next == NULL) {
      prev_node->next = new_node;
      new_node->next = NULL;
    } else {
      new_node->next = prev_node->next;
      prev_node->next = new_node;
    }
  }
}

static void free_list_node_remove(free_list_node **phead, free_list_node *prev_node,
                                  free_list_node *del_node) {
  if (prev_node == NULL) {
    *phead = del_node->next;
  } else {
    prev_node->next = del_node->next;
  }
}

mem_allocator allocator_arena_init(uintptr_t size) {
  mem_allocator alloc = {
      .type = ALLOCATOR_TYPE_ARENA,
      .arena = arena_init(size),
  };
  return alloc;
}

mem_allocator allocator_free_list_init(uintptr_t size) {
  mem_allocator alloc = {
      .type = ALLOCATOR_TYPE_FREE_LIST,
      .free_list = free_list_init(size),
  };
  return alloc;
}

void allocator_destroy(mem_allocator *allocator) {
  switch (allocator->type) {
  case ALLOCATOR_TYPE_ARENA:
    arena_free(&allocator->arena);
    break;
  case ALLOCATOR_TYPE_FREE_LIST:
    free_list_free(&allocator->free_list);
    break;
  }
}

void *allocator_alloc_impl(mem_allocator *allocator, uintptr_t size, uintptr_t alignment) {
  switch (allocator->type) {
  case ALLOCATOR_TYPE_ARENA:
    return arena_alloc(&allocator->arena, size, alignment);
  case ALLOCATOR_TYPE_FREE_LIST:
    return free_list_alloc(&allocator->free_list, size, alignment);
  }
  return NULL;
}

void allocator_dealloc(mem_allocator *allocator, void *data) {
  switch (allocator->type) {
  case ALLOCATOR_TYPE_ARENA:
    break;
  case ALLOCATOR_TYPE_FREE_LIST:
    free_list_dealloc(&allocator->free_list, data);
  }
}

void allocator_clear(mem_allocator *allocator) {
  switch (allocator->type) {
  case ALLOCATOR_TYPE_ARENA:
    arena_clear(&allocator->arena);
    break;
  case ALLOCATOR_TYPE_FREE_LIST:
    break;
  }
}
