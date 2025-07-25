#ifndef ASSET_H
#define ASSET_H

#include "mem.hpp"
#include "miniaudio.h"
#include <glm/glm.hpp>

struct asset_image {
  glm::vec2 size;
  unsigned char *data;
  uint32_t texture_id;
};
asset_image asset_load_image(const char *path, mem_allocator *allocator, mem_allocator *temp_allocator);
void asset_delete_image(asset_image *image, mem_allocator *allocator);

struct asset_sound {
  unsigned char *data;
  ma_sound *sound;
  ma_decoder *decoder;
};
asset_sound asset_load_sound(const char *path, ma_engine *audio_player, mem_allocator *allocator);
void asset_delete_sound(asset_sound *wav, mem_allocator *allocator);

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
asset_font asset_load_font(const char *path, float height, mem_allocator *allocator,
                           mem_allocator *temp_allocator);
void asset_delete_font(asset_font *font, mem_allocator *allocator);
#endif
