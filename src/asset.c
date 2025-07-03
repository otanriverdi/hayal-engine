#include "asset.h"
#include "mem.h"
#include "platform.h"
#include <miniaudio.h>
#include <stb_image.h>
#include <stdalign.h>
#include <string.h>

loaded_png load_image(char *path, free_list *allocator, arena *temp_allocator) {
  size_t file_size;
  platform_get_file_size(path, &file_size);
  unsigned char *file_memory = arena_alloc(temp_allocator, file_size, alignof(unsigned char));
  platform_read_entire_file(path, file_size, file_memory);

  int x, y, n;
  unsigned char *pixels = stbi_load_from_memory(file_memory, file_size, &x, &y, &n, 4);
  size_t pixels_size = x * y * 4;

  loaded_png png = {
      .size = {.x = x, .y = y},
      .data = free_list_alloc(allocator, pixels_size, alignof(unsigned char)),
      .texture_id = 0,
  };

  memcpy(png.data, pixels, pixels_size);
  stbi_image_free(pixels);

  return png;
}

loaded_wav load_wav(char *path, int channels, int freq, free_list *free_list, arena *temp_arena) {
  size_t file_size;
  platform_get_file_size(path, &file_size);
  unsigned char *file_memory = arena_alloc(temp_arena, file_size, alignof(unsigned char));
  platform_read_entire_file(path, file_size, file_memory);

  ma_decoder_config decoder_config = ma_decoder_config_init(ma_format_f32, channels, freq);
  ma_decoder decoder;
  ma_decoder_init_memory(file_memory, file_size, &decoder_config, &decoder);

  ma_uint64 frame_count;
  ma_decoder_get_length_in_pcm_frames(&decoder, &frame_count);

  loaded_wav wav = {.frame_count = frame_count,
                    .data = free_list_alloc(free_list, frame_count * channels, alignof(float))};

  ma_uint64 frames_read;
  ma_decoder_read_pcm_frames(&decoder, wav.data, frame_count, &frames_read);

  ma_decoder_uninit(&decoder);

  return wav;
}
