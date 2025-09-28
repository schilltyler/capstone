
#ifndef PROCFS_UTIL_H
#define PROCFS_UTIL_H
#include "minstr.h"
#include "syscall.h"
#include "sysdebug.h"
#include <fcntl.h>
#include <stddef.h> // for ptrdiff_t, size_t
#include <stdint.h>
#include <stdio.h>

// Added the new string as requested
#define STACK_STR_PROC_SELF_MAPS(var)                                          \
  char var[16];                                                                \
  var[0] = '/';                                                                \
  var[1] = 'p';                                                                \
  var[2] = 'r';                                                                \
  var[3] = 'o';                                                                \
  var[4] = 'c';                                                                \
  var[5] = '/';                                                                \
  var[6] = 's';                                                                \
  var[7] = 'e';                                                                \
  var[8] = 'l';                                                                \
  var[9] = 'f';                                                                \
  var[10] = '/';                                                               \
  var[11] = 'm';                                                               \
  var[12] = 'a';                                                               \
  var[13] = 'p';                                                               \
  var[14] = 's';                                                               \
  var[15] = '\0';

/**
 * Search for a library in /proc/self/maps and retrieve its base address
 *
 * @param lib_needle The library name to search for
 * @param prot The protection string to match (e.g. "r-xp")
 * @param base_out Pointer where the base address will be stored
 * @param lib_path Buffer where the full path will be stored (if not NULL)
 * @param path_max Maximum size of the lib_path buffer
 *
 * @return 0 on success, -1 on failure
 */
static int search_lib_procmaps(char *lib_needle, char *prot,
                               uintptr_t *base_out, char *lib_path,
                               size_t path_max) {
  char buf[4096];
  int fd, bytes_read, i, line_start, found = 0;
  uintptr_t addr_start;

  DEBUG_WRITE_LOG("Searching through  /proc/self/maps\n");
  // TODO:
  DEBUG_WRITE_LOG(" !!!!!!!!!!!NOT IMPLEMENTED\n");
  return found ? 0 : -1;
}

#endif // PROCFS_UTIL_H
