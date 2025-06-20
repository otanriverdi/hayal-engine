#ifndef HAYAL_MEM
#define HAYAL_MEM

#include <stdint.h>

typedef struct Arena {
  uintptr_t cursor;
  uintptr_t size;
  void *ptr;
} Arena;
Arena ArenaInit(void *buffer, uintptr_t size);
void *ArenaAlloc(Arena *arena, uintptr_t size, uintptr_t alignement);
void ArenaClear(Arena *arena);
void ArenaFree(Arena *arena);

typedef struct FreeListAllocHeader {
  uintptr_t block_size;
  uintptr_t padding;
} FreeListAllocHeader;

typedef struct FreeListNode FreeListNode;
struct FreeListNode {
  FreeListNode *next;
  uintptr_t block_size;
};

typedef struct FreeList {
  void *data;
  uintptr_t size;
  uintptr_t used;

  FreeListNode *head;
} FreeList;

FreeList FreeListInit(void *buffer, uintptr_t size);
void *FreeListAlloc(FreeList *fl, uintptr_t size, uintptr_t alignment);
void FreeListDealloc(FreeList *fl, void *data);
void FreeListFree(FreeList *fl);

#endif
