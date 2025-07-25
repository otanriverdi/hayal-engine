#include "game/asset.hpp"
#include "mem.hpp"
#include "platform.hpp"
#include <ft2build.h>
#include <stb_image.h>
#include FT_FREETYPE_H

asset_image asset_load_image(const char *path, mem_allocator *allocator, mem_allocator *temp_allocator) {
  size_t file_size;
  unsigned char *file_memory;
  platform_load_entire_file_with_arena(path, temp_allocator, &file_memory, &file_size);

  int x, y, n;
  unsigned char *pixels = stbi_load_from_memory(file_memory, file_size, &x, &y, &n, 4);
  size_t pixels_size = x * y * 4;

  void *buffer = allocator_alloc(allocator, unsigned char, pixels_size);
  assert(buffer != NULL);
  asset_image png = {
      .size = glm::vec2(static_cast<float>(x), static_cast<float>(y)),
      .data = static_cast<unsigned char *>(buffer),
      .texture_id = 0,
  };

  memcpy(png.data, pixels, pixels_size);
  stbi_image_free(pixels);

  return png;
}

void asset_delete_image(asset_image *image, mem_allocator *allocator) {
  if (image->texture_id > 0) {
    platform_log_debug("Asset image deleted with dangling texture: %i", image->texture_id);
  }
  if (image->data != NULL) {
    allocator_dealloc(allocator, image->data);
    image->data = NULL;
  }
};

asset_sound asset_load_sound(const char *path, ma_engine *audio_player, mem_allocator *allocator) {
  asset_sound sound;
  size_t size;
  platform_load_entire_file_with_free_list(path, allocator, &sound.data, &size);
  sound.decoder = allocator_alloc(allocator, ma_decoder, 1);
  sound.sound = allocator_alloc(allocator, ma_sound, 1);
  ma_result result = ma_decoder_init_memory(sound.data, size, NULL, sound.decoder);
  assert(result == MA_SUCCESS);
  result = ma_sound_init_from_data_source(audio_player, sound.decoder, 0, NULL, sound.sound);
  assert(result == MA_SUCCESS);

  return sound;
}

void asset_delete_sound(asset_sound *sound, mem_allocator *allocator) {
  ma_decoder_uninit(sound->decoder);
  ma_sound_uninit(sound->sound);
  allocator_dealloc(allocator, sound->sound);
  allocator_dealloc(allocator, sound->decoder);
  allocator_dealloc(allocator, sound->data);
}

asset_font asset_load_font(const char *path, float height, mem_allocator *allocator,
                           mem_allocator *temp_allocator) {
  size_t file_size;
  unsigned char *file_memory;
  platform_load_entire_file_with_arena(path, temp_allocator, &file_memory, &file_size);

  FT_Library ft;
  assert(FT_Init_FreeType(&ft) == 0);
  FT_Face face;
  assert(FT_New_Memory_Face(ft, file_memory, file_size, 0, &face) == 0);
  FT_Set_Pixel_Sizes(face, 0, height);

  asset_font font = {};
  for (unsigned char c = 0; c < ASSET_FONT_NUM_CHARS; c++) {
    assert(FT_Load_Char(face, c, FT_LOAD_RENDER) == 0);
    assert(face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
    asset_font_char ch = (asset_font_char){.texture_id = 0,
                                           .size = glm::vec2(static_cast<float>(face->glyph->bitmap.width),
                                                             static_cast<float>(face->glyph->bitmap.rows)),
                                           .bearing = glm::vec2(static_cast<float>(face->glyph->bitmap_left),
                                                                static_cast<float>(face->glyph->bitmap_top)),
                                           .advance = static_cast<uint32_t>(face->glyph->advance.x)};
    size_t buffer_size = abs(face->glyph->bitmap.pitch) * face->glyph->bitmap.rows;
    ch.data = allocator_alloc(allocator, unsigned char, buffer_size);
    memcpy(ch.data, face->glyph->bitmap.buffer, buffer_size);
    font.characters[c] = ch;
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
  return font;
}

void asset_delete_font(asset_font *font, mem_allocator *allocator) {
  for (unsigned char c = 0; c < ASSET_FONT_NUM_CHARS; c++) {
    if (font->characters[c].data == NULL) {
      continue;
    }
    if (font->characters[c].texture_id > 0) {
      platform_log_debug("Font character %d deleted with dangling texture: %i", c,
                         font->characters[c].texture_id);
    }
    allocator_dealloc(allocator, font->characters[c].data);
    font->characters[c].data = NULL;
  }
}
