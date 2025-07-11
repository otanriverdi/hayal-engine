#include "asset.h"
#include "mem.h"
#include "platform.h"
#include <assert.h>
#include <ft2build.h>
#include <miniaudio.h>
#include <stb_image.h>
#include <stdalign.h>
#include <stdlib.h>
#include FT_FREETYPE_H

asset_image asset_load_image(char *path, free_list *allocator, arena *temp_allocator) {
  size_t file_size;
  unsigned char *file_memory;
  platform_load_entire_file(path, temp_allocator, &file_memory, &file_size);

  int x, y, n;
  unsigned char *pixels = stbi_load_from_memory(file_memory, file_size, &x, &y, &n, 4);
  size_t pixels_size = x * y * 4;

  void *buffer = free_list_alloc(allocator, pixels_size, alignof(unsigned char));
  assert(buffer != NULL);
  asset_image png = {
      .size = {.x = x, .y = y},
      .data = buffer,
      .texture_id = 0,
  };

  memcpy(png.data, pixels, pixels_size);
  stbi_image_free(pixels);

  return png;
}

void asset_delete_image(asset_image *image, free_list *allocator) {
  if (image->data != NULL) {
    free_list_dealloc(allocator, image->data);
    image->data = NULL;
  }
};

asset_wav asset_load_wav(char *path, int channels, int freq, free_list *allocator, arena *temp_allocator) {
  size_t file_size;
  unsigned char *file_memory;
  platform_load_entire_file(path, temp_allocator, &file_memory, &file_size);

  ma_decoder_config decoder_config = ma_decoder_config_init(ma_format_f32, channels, freq);
  ma_decoder decoder;
  ma_decoder_init_memory(file_memory, file_size, &decoder_config, &decoder);

  ma_uint64 frame_count;
  ma_decoder_get_length_in_pcm_frames(&decoder, &frame_count);

  void *buffer = free_list_alloc(allocator, frame_count * channels, alignof(float));
  assert(buffer != NULL);
  asset_wav wav = {.frame_count = frame_count, .data = buffer};

  ma_uint64 frames_read;
  ma_decoder_read_pcm_frames(&decoder, wav.data, frame_count, &frames_read);

  ma_decoder_uninit(&decoder);

  return wav;
}

void asset_delete_wav(asset_wav *wav, free_list *allocator) {
  if (wav->data != NULL) {
    free_list_dealloc(allocator, wav->data);
    wav->data = NULL;
  }
}

asset_font asset_load_font(char *path, float height, free_list *allocator, arena *temp_allocator) {
  size_t file_size;
  unsigned char *file_memory;
  platform_load_entire_file(path, temp_allocator, &file_memory, &file_size);

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
                                           .size =
                                               (vec2){
                                                   .x = face->glyph->bitmap.width,
                                                   .y = face->glyph->bitmap.rows,
                                               },
                                           .bearing =
                                               (vec2){
                                                   .x = face->glyph->bitmap_left,
                                                   .y = face->glyph->bitmap_top,
                                               },
                                           .advance = face->glyph->advance.x};
    size_t buffer_size = abs(face->glyph->bitmap.pitch) * face->glyph->bitmap.rows;
    ch.data = free_list_alloc(allocator, buffer_size, alignof(unsigned char));
    memcpy(ch.data, face->glyph->bitmap.buffer, buffer_size);
    font.characters[c] = ch;
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
  return font;
}

void asset_delete_font(asset_font *font, free_list *allocator) {
  for (unsigned char c = 0; c < ASSET_FONT_NUM_CHARS; c++) {
    if (font->characters[c].data == NULL) {
      continue;
    }
    free_list_dealloc(allocator, font->characters[c].data);
    font->characters[c].data = NULL;
  }
}
