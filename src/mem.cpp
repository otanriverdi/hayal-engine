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
  fl.size = size;

  free_list_node *initial_node = (free_list_node *)fl.data;
  initial_node->block_size = size;
  initial_node->next = NULL;

  fl.head = initial_node;
  fl.used = 0;

  return fl;
}

static size_t calculate_required_padding(uintptr_t ptr, uintptr_t alignment, size_t header_size) {
  uintptr_t aligned_address = (ptr + header_size + alignment - 1) & ~(alignment - 1);
  return aligned_address - ptr;
}

static void insert_free_node(free_list_node **head, free_list_node *prev_node, free_list_node *new_node) {
  if (prev_node == NULL) {
    new_node->next = *head;
    *head = new_node;
  } else {
    new_node->next = prev_node->next;
    prev_node->next = new_node;
  }
}

static void remove_free_node(free_list_node **head, free_list_node *prev_node,
                             free_list_node *node_to_remove) {
  if (prev_node == NULL) {
    *head = node_to_remove->next;
  } else {
    prev_node->next = node_to_remove->next;
  }
}

static void *free_list_alloc(free_list *fl, uintptr_t size, uintptr_t alignment) {
  // enforce minimum size and alignment to prevent fragmentation
  if (size < sizeof(free_list_node)) {
    size = sizeof(free_list_node);
  }
  if (alignment < 8) {
    alignment = 8;
  }

  free_list_node *node = fl->head;
  free_list_node *prev_node = NULL;
  while (node != NULL) {
    // calculate padding needed to fit the header
    size_t padding = calculate_required_padding((uintptr_t)node, alignment, sizeof(free_list_alloc_header));
    size_t total_required = size + padding;

    if (node->block_size >= total_required) {
      break;
    }

    prev_node = node;
    node = node->next;
  }

  assert(node != NULL && "Out of memory");

  // calculate final padding and space requirements
  size_t total_padding =
      calculate_required_padding((uintptr_t)node, alignment, sizeof(free_list_alloc_header));
  size_t alignment_padding = total_padding - sizeof(free_list_alloc_header);
  size_t total_required_space = size + total_padding;

  // split the block if there's leftover space worth keeping
  size_t remaining_space = node->block_size - total_required_space;
  if (remaining_space > 0) {
    free_list_node *leftover_node = (free_list_node *)((char *)node + total_required_space);
    leftover_node->block_size = remaining_space;
    insert_free_node(&fl->head, node, leftover_node);
  }

  // remove the chosen node from the free list
  remove_free_node(&fl->head, prev_node, node);

  // set up the allocation header with size and padding info
  free_list_alloc_header *header = (free_list_alloc_header *)((char *)node + alignment_padding);
  header->block_size = total_required_space;
  header->padding = alignment_padding;

  fl->used += total_required_space;
  return (void *)((char *)header + sizeof(free_list_alloc_header));
}

static void free_list_dealloc(free_list *fl, void *ptr) {
  if (ptr == NULL) {
    return;
  }

  // reconstruct the original block from the allocation header
  free_list_alloc_header *header = (free_list_alloc_header *)((char *)ptr - sizeof(free_list_alloc_header));
  free_list_node *freed_node = (free_list_node *)((char *)header - header->padding);
  freed_node->block_size = header->block_size;
  freed_node->next = NULL;

  // find where to insert this node in address-sorted order
  free_list_node *current = fl->head;
  free_list_node *prev = NULL;
  while (current != NULL && freed_node > current) {
    prev = current;
    current = current->next;
  }

  insert_free_node(&fl->head, prev, freed_node);

  fl->used -= freed_node->block_size;

  // try to merge with the next block if they're adjacent
  if (freed_node->next != NULL && (void *)((char *)freed_node + freed_node->block_size) == freed_node->next) {
    freed_node->block_size += freed_node->next->block_size;
    remove_free_node(&fl->head, freed_node, freed_node->next);
  }

  // try to merge with the previous block if they're adjacent
  if (prev != NULL && (void *)((char *)prev + prev->block_size) == freed_node) {
    prev->block_size += freed_node->block_size;
    remove_free_node(&fl->head, prev, freed_node);
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
