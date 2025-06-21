#ifndef ASSET_H
#define ASSET_H

#include "math.h"
#include "mem.h"

typedef struct loaded_png {
  vec2 size;
  unsigned char *data;
  uint32_t texture_id;
} loaded_png;
loaded_png load_png(char *path, free_list *free_list, arena *temp_arena);

#endif
