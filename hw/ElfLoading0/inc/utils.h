#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>

// String and memory utilities
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
char *strcpy(char *dest, const char *src);
size_t strlen(const char *s);

// Mini printf with limited format specifiers
// Supports: %s, %d, %x, %p, %%
void mini_printf(const char *fmt, ...);

// Hex dump utility for debugging
void print_hex_dump(const void *data, size_t size);

#endif /* UTILS_H */
