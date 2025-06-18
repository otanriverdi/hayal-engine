#include "mem.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

Arena ArenaInit(void *buffer, uintptr_t size) {
  Arena arena = {};
  arena.size = size;
  arena.ptr = buffer;
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

FreeList FreeListInit(void *buffer, uintptr_t size) {
  FreeList fl;

  if (buffer == NULL || size < sizeof(FreeListNode)) {
    fl.head = NULL;
    fl.used = 0;
  }

  FreeListNode *initial_node = (FreeListNode *)buffer;
  initial_node->block_size = size;
  initial_node->next = NULL;

  fl.head = initial_node;
  fl.used = 0;

  return fl;
}

static size_t CalcPaddingWithHeader(uintptr_t ptr, uintptr_t alignment,
                                    size_t header_size);
static void FreeListNodeInsert(FreeListNode **phead, FreeListNode *prev_node,
                               FreeListNode *new_node);
static void FreeListNodeRemove(FreeListNode **phead, FreeListNode *prev_node,
                               FreeListNode *del_node);

void *FreeListAlloc(FreeList *fl, uintptr_t size, uintptr_t alignment) {
  // we enforce a minimum size and alignment
  if (size < sizeof(FreeListNode)) {
    size = sizeof(FreeListNode);
  }
  if (alignment < 8) {
    alignment = 8;
  }

  // find the suitable node
  size_t padding = 0;
  FreeListNode *node = fl->head;
  FreeListNode *prev_node = NULL;
  while (node != NULL) {
    // determine how much more padding we need to fit the header
    padding = CalcPaddingWithHeader((uintptr_t)node, alignment,
                                    sizeof(FreeListAllocHeader));
    size_t required = size + padding;
    if (node->block_size >= required) {
      break;
    }
    prev_node = node;
    node = node->next;
  }

  if (node == NULL) {
    return NULL;
  }

  // padding before the header
  size_t alignment_padding = padding - sizeof(FreeListAllocHeader);
  size_t required_space = size + padding;
  size_t remaining_space = node->block_size - required_space;

  // there is empty space in the node we can create a new block from
  if (remaining_space > 0) {
    FreeListNode *new_node = (FreeListNode *)((char *)node + required_space);
    new_node->block_size = remaining_space;
    FreeListNodeInsert(&fl->head, node, new_node);
  }

  FreeListNodeRemove(&fl->head, prev_node, node);

  // we insert the header to the location we reserved for it
  FreeListAllocHeader *header_ptr =
      (FreeListAllocHeader *)((char *)node + alignment_padding);
  header_ptr->block_size = required_space;
  header_ptr->padding = alignment_padding;

  // return the pointer after the header where the usable memory begins
  return (void *)((char *)header_ptr + sizeof(FreeListAllocHeader));
}

void FreeListDealloc(FreeList *fl, void *ptr) {
  if (ptr == NULL) {
    return;
  }

  FreeListAllocHeader *header =
      (FreeListAllocHeader *)((char *)ptr - sizeof(FreeListAllocHeader));
  FreeListNode *free_node = (FreeListNode *)header;
  free_node->block_size = header->block_size + header->padding;
  free_node->next = NULL;

  FreeListNode *node = fl->head;
  FreeListNode *prev_node = NULL;
  while (node != NULL) {
    if ((FreeListNode *)ptr < node) {
      FreeListNodeInsert(&fl->head, prev_node, free_node);
      break;
    }
    prev_node = node;
    node = node->next;
  }

  fl->used = free_node->block_size;

  if (free_node->next != NULL &&
      (void *)((char *)free_node + free_node->block_size) == free_node->next) {
    free_node->block_size += free_node->next->block_size;
    FreeListNodeRemove(&fl->head, free_node, free_node->next);
  }

  if (prev_node != NULL && prev_node->next != NULL &&
      (void *)((char *)prev_node + prev_node->block_size) == free_node) {
    prev_node->block_size += free_node->next->block_size;
    FreeListNodeRemove(&fl->head, prev_node, free_node);
  }
}

void FreeListFree(FreeList *fl) {
  if (fl == NULL) {
    return;
  }

  fl->head = NULL;
  fl->used = 0;
  free(fl->data);
}

static size_t CalcPaddingWithHeader(uintptr_t ptr, uintptr_t alignment,
                                    size_t header_size) {
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
static void FreeListNodeInsert(FreeListNode **phead, FreeListNode *prev_node,
                               FreeListNode *new_node) {
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

static void FreeListNodeRemove(FreeListNode **phead, FreeListNode *prev_node,
                               FreeListNode *del_node) {
  if (prev_node == NULL) {
    *phead = del_node->next;
  } else {
    prev_node->next = del_node->next;
  }
}
