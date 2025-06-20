#include "platform.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void platform_get_file_size(char *path, size_t *size) {
  struct stat st;
  if (stat(path, &st) == -1) {
    fprintf(stderr, "Error getting file size for %s: %s\n", path,
            strerror(errno));
    *size = 0;
    return;
  }
  *size = st.st_size;
}

void platform_read_entire_file(char *path, size_t size, void *out) {
  FILE *file = fopen(path, "rb");
  if (!file) {
    fprintf(stderr, "Error opening file %s for reading: %s\n", path,
            strerror(errno));
    return;
  }

  size_t bytes_read = fread(out, 1, size, file);
  if (bytes_read != size) {
    if (feof(file)) {
      fprintf(stderr, "Unexpected end of file %s (read %zu, expected %zu)\n",
              path, bytes_read, size);
    } else {
      fprintf(stderr, "Error reading file %s: %s\n", path, strerror(errno));
    }
  }

  fclose(file);
}

void platform_write_file(char *path, size_t size, void *data) {
  FILE *file = fopen(path, "wb");
  if (!file) {
    fprintf(stderr, "Error opening file %s for writing: %s\n", path,
            strerror(errno));
    return;
  }

  size_t bytes_written = fwrite(data, 1, size, file);
  if (bytes_written != size) {
    fprintf(stderr, "Error writing file %s (wrote %zu, expected %zu): %s\n",
            path, bytes_written, size, strerror(errno));
  }

  fclose(file);
}
