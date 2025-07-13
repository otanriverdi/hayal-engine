#ifndef ASSET_H
#define ASSET_H

#include "linalg.h"
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
asset_wav asset_load_wav(char *path, int channels, int freq, free_list *allocator, arena *temp_allocator);
void asset_delete_wav(asset_wav *wav, free_list *allocator);

#define ASSET_FONT_NUM_CHARS 128
typedef struct asset_font_char {
  uint32_t texture_id;
  vec2 size;
  vec2 bearing;
  uint32_t advance;
  unsigned char *data;
} asset_font_char;

typedef struct asset_font {
  asset_font_char characters[ASSET_FONT_NUM_CHARS];
} asset_font;
asset_font asset_load_font(char *path, float height, free_list *allocator, arena *temp_allocator);
void asset_delete_font(asset_font *font, free_list *allocator);
#endif
