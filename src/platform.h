#ifndef HAYAL_PLATFORM
#define HAYAL_PLATFORM

#include <stdbool.h>
#include <stddef.h>

void PlatformGetFileSize(char *type, size_t *size);
void PlatformReadEntireFile(char *type, size_t size, void *out);
void PlatformWriteFile(char *type, size_t size, void *out);

#endif
