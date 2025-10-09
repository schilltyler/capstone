#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>

size_t strlen(const char *s) {
  size_t length = 0;
  while (*s != '\0') {
    length++;
    s++;
  }
  return length;
}

#endif
