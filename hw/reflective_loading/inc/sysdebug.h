
#ifndef SYSDEBUG_H
#define SYSDEBUG_H

#ifdef DEBUG
#include "syscall.h" // For sys_write
/**
 * Write a debug message to stderr (file descriptor 2)
 * This function is only active when DEBUG is defined
 *
 * @param msg Null-terminated message to write
 */
#define DEBUG_WRITE_LOG(msg)                                                   \
  do {                                                                         \
    const char *__debug_msg = (msg);                                           \
    size_t __debug_len = 0;                                                    \
    while (__debug_msg[__debug_len])                                           \
      __debug_len++;                                                           \
    sys_write(2, __debug_msg, __debug_len);                                    \
  } while (0)

#else
/**
 * No-op version of DEBUG_WRITE_LOG for when DEBUG is not defined
 */
#define DEBUG_WRITE_LOG(msg)                                                   \
  do {                                                                         \
  } while (0)
#endif

#endif /* SYSDEBUG_H */
