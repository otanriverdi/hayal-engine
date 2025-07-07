#ifndef ASSET_H
#define ASSET_H

#include "math.h"
#include "mem.h"

typedef struct asset_image {
  vec2 size;
  unsigned char *data;
  uint32_t texture_id;
} asset_image;
asset_image asset_load_image(char *path, free_list *allocator, arena *temp_allocator);
void asset_delete_image(asset_image *image, free_list *allocator);

typedef struct asset_wav {
  float *data;
  uint64_t frame_count;
} asset_wav;
asset_wav asset_load_wav(char *path, int channels, int freq, free_list *free_list, arena *temp_arena);

#endif
