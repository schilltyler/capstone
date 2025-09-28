
#ifndef MINSTR_H
#define MINSTR_H

#include <stdint.h>

/**
 * Find the first occurrence of a substring in a string.
 *
 * @param haystack String to search in
 * @param needle String to search for
 * @return Pointer to the first occurrence of needle in haystack, or NULL if not
 * found
 */
static char *strstr(const char *haystack, const char *needle) { return 0; }

/**
 * Fill memory with a constant byte.
 *
 * @param s Memory area to fill
 * @param c Constant byte to fill with
 * @param n Number of bytes to fill
 * @return Pointer to the memory area s
 */
static void *memset(void *s, int c, unsigned long n) {}

/**
 * Copy memory area.
 *
 * @param dest Destination memory area
 * @param src Source memory area
 * @param n Number of bytes to copy
 * @return Pointer to the destination memory area
 */
static void *memcpy(void *dest, const void *src, unsigned long n) {}

/**
 * Compare memory areas.
 *
 * @param s1 First memory area
 * @param s2 Second memory area
 * @param n Number of bytes to compare
 * @return <0 if s1 is less than s2, 0 if equal, >0 if s1 is greater than s2
 */
static int memcmp(const void *s1, const void *s2, unsigned long n) { return 0; }

/**
 * Calculate the length of a string.
 *
 * @param s String to measure
 * @return Length of the string
 */
static unsigned long strlen(const char *s) {}

/**
 * Calculate the length of a string with maximum length.
 *
 * @param s String to measure
 * @param maxlen Maximum length to check
 * @return Length of the string or maxlen if no null terminator is found
 */
static unsigned long strnlen(const char *s, unsigned long maxlen) {}

/**
 * Compare two strings.
 *
 * @param s1 First string
 * @param s2 Second string
 * @return <0 if s1 is less than s2, 0 if equal, >0 if s1 is greater than s2
 */
static int strcmp(const char *s1, const char *s2) {}

/**
 * Convert a hexadecimal string to an unsigned 64-bit integer.
 *
 * @param s Hexadecimal string
 * @return Converted integer value
 */
static uintptr_t hex_str_to_u64(const char *s) {
  uintptr_t val = 0;
  return val;
}

#endif /* MINSTR_H */
