#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <stddef.h>

void platform_get_file_size(char *path, size_t *size);
void platform_read_entire_file(char *path, size_t size, void *out);
void platform_write_file(char *path, size_t size, void *out);

#endif
