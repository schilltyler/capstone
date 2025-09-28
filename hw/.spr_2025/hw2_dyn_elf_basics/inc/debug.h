#define DEBUG
#ifdef DEBUG
#include <elf.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

// A debug_printf that writes to stderr.
void debug_printf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}

#else
// When DEBUG is not defined, make these functions no-ops.
#define debug_printf(...) ((void)0)
#endif
