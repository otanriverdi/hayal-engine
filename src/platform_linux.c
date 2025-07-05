#include "platform.h"
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>

void platform_get_file_size(char *path, size_t *size) {
  struct stat st;
  int res = stat(path, &st);
  assert(res != -1);
  *size = st.st_size;
}

void platform_read_entire_file(char *path, size_t size, void *out) {
  FILE *file = fopen(path, "rb");
  assert(file);
  size_t bytes_read = fread(out, 1, size, file);
  assert(bytes_read == size);
  fclose(file);
}

void platform_write_file(char *path, size_t size, void *data) {
  FILE *file = fopen(path, "wb");
  assert(file);
  size_t bytes_written = fwrite(data, 1, size, file);
  assert(bytes_written == size);
  fclose(file);
}
