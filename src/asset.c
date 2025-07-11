#include "asset.h"
#include "mem.h"
#include "platform.h"
#include <assert.h>
#include <miniaudio.h>
#include <stb_image.h>
#include <stdalign.h>
#include <string.h>

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
  if (image->texture_id > 0) {
    platform_log_debug("Asset image deleted with dangling texture: %i", image->texture_id);
  }
  free_list_dealloc(allocator, image->data);
  image->data = NULL;
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

void asset_delete_wav(asset_wav *wav, free_list *allocator) { free_list_dealloc(allocator, wav->data); }
