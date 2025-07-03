#ifndef ASSET_H
#define ASSET_H

#include "math.h"
#include "mem.h"

typedef struct loaded_image {
  vec2 size;
  unsigned char *data;
  uint32_t texture_id;
} loaded_png;
loaded_png load_image(char *path, free_list *allocator, arena *temp_allocator);

typedef struct loaded_wav {
  float *data;
  uint64_t frame_count;
} loaded_wav;
loaded_wav load_wav(char *path, int channels, int freq, free_list *free_list, arena *temp_arena);

#endif
