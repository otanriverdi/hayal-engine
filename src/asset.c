#include "asset.h"
#include "platform.h"
#include <stb_image.h>
#include <stdalign.h>
#include <string.h>

loaded_png load_png(char *path, free_list *free_list, arena *temp_arena) {
  size_t file_size;
  platform_get_file_size(path, &file_size);
  unsigned char *file_memory =
      arena_alloc(temp_arena, file_size, alignof(unsigned char));
  platform_read_entire_file(path, file_size, file_memory);

  int x, y, n;
  unsigned char *pixels =
      stbi_load_from_memory(file_memory, file_size, &x, &y, &n, 4);
  size_t pixels_size = x * y * n;

  loaded_png png = {
      .size = (vec2){.x = x, .y = y},
      .data = free_list_alloc(free_list, pixels_size, alignof(unsigned char)),
      .texture_id = 0,
  };

  memcpy(png.data, pixels, pixels_size);

  return png;
}
