

#include "reflective_loader.h"
// #include "reflective_loader.h"
#include "sysdebug.h"
#include <stdio.h>
#include <stdlib.h>

extern void *memfd_load;

static void libc_sub_call() { printf("Look mah! a libc subroutune!\n"); }

void __attribute__((constructor)) payload2() {
  DEBUG_WRITE_LOG("well here go again\n");
  printf("Hello ther!\n");
  libc_sub_call();
}

void __attribute__((constructor)) payload() {
  DEBUG_WRITE_LOG("well here goes noting\n");
  printf("Payload!!: %p\n", (void *)&payload);
  void *x = malloc(0);
  free(x);
}
