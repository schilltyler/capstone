#ifndef RUNTIME_H
#define RUNTIME_H

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
size_t strlen(const char *s);

void write_str(int fd, const char *s);
void write_char(int fd, char c);

void mini_printf(const char *fmt, ...);

void die(const char *msg);
void die_with_status(const char *msg, int status);

#endif /* RUNTIME_H */
