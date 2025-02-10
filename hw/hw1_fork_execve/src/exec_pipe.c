#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// optional  for debugging
#include <errno.h>
/*
================================================================================
Part 1: Process Creation and I/O Redirection (Pipe Version for AArch64)
================================================================================
*/

// External declaration for environment variables
// set by runtime linker
extern char **environ;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <executable> [args...]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
    return 0;
}
