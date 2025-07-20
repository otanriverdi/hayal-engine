#ifndef ASSET_H
#define ASSET_H

#include "mem.hpp"
#include <glm/glm.hpp>

struct asset_image {
  glm::vec2 size;
  unsigned char *data;
  uint32_t texture_id;
};
asset_image asset_load_image(const char *path, free_list *allocator, arena *temp_allocator);
void asset_delete_image(asset_image *image, free_list *allocator);

struct asset_wav {
  float *data;
  uint64_t frame_count;
};
asset_wav asset_load_wav(const char *path, int channels, int freq, free_list *allocator,
                         arena *temp_allocator);
void asset_delete_wav(asset_wav *wav, free_list *allocator);

#define ASSET_FONT_NUM_CHARS 128
struct asset_font_char {
  uint32_t texture_id;
  glm::vec2 size;
  glm::vec2 bearing;
  uint32_t advance;
  unsigned char *data;
};

struct asset_font {
  asset_font_char characters[ASSET_FONT_NUM_CHARS];
};
asset_font asset_load_font(const char *path, float height, free_list *allocator, arena *temp_allocator);
void asset_delete_font(asset_font *font, free_list *allocator);
#endif
