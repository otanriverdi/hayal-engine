#include "platform.hpp"
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>

void platform_get_file_size(const char *path, size_t *size) {
  struct stat st;
  int res = stat(path, &st);
  assert(res != -1);
  *size = st.st_size;
}

void platform_read_entire_file(const char *path, size_t size, void *out) {
  FILE *file = fopen(path, "rb");
  assert(file);
  size_t bytes_read = fread(out, 1, size, file);
  assert(bytes_read == size);
  fclose(file);
}

void platform_write_file(const char *path, size_t size, void *data) {
  FILE *file = fopen(path, "wb");
  assert(file);
  size_t bytes_written = fwrite(data, 1, size, file);
  assert(bytes_written == size);
  fclose(file);
}

void platform_log_info(const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  printf("[INFO] ");
  vprintf(msg, args);
  printf("\n");
  va_end(args);
}

void platform_log_debug(const char *msg, ...) {
#ifndef NDEBUG
  va_list args;
  va_start(args, msg);
  printf("[DEBUG] ");
  vprintf(msg, args);
  printf("\n");
  va_end(args);
#endif
}

void platform_log_error(const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  fprintf(stderr, "[ERROR] ");
  vfprintf(stderr, msg, args);
  fprintf(stderr, "\n");
  va_end(args);
}
