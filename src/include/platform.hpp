#ifndef PLATFORM_H
#define PLATFORM_H

#include "assert.h"
#include "mem.hpp"
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>

void platform_get_file_size(const char *path, size_t *size);
void platform_read_entire_file(const char *path, size_t size, void *out);
void platform_write_file(const char *path, size_t size, void *out);
void platform_log_info(const char *msg, ...);
void platform_log_debug(const char *msg, ...);
void platform_log_error(const char *msg, ...);

/** This is a high-level helper on top of the low-level platform functions. If you need tighter control on
 * memory, prefer the low level functions.
 */
static inline void platform_load_entire_file(const char *path, arena *temp_allocator, unsigned char **buffer,
                                             size_t *size) {
  platform_get_file_size(path, size);
  *buffer = static_cast<unsigned char *>(arena_alloc(temp_allocator, *size, alignof(unsigned char)));
  assert(*buffer != NULL);
  platform_read_entire_file(path, *size, *buffer);
};

#endif
