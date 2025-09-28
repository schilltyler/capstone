#include "sysdebug.h"
#include <sys/types.h>
// this is a sample program for
// executing an elf file that
// has not been loaded by
// ld.so.

void *sample(size_t n_iters) {
  for (size_t i = 0; i < n_iters; i++) {
    DEBUG_WRITE_LOG("[+] Look mah! no loader\n");
  }
  return (void *)&sample;
}
