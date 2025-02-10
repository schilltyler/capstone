// procfs.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long long find_lib_addr(char *lib) {
  }

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: ./%s <lib_name>", argv[0]);
    return 1;
  }
  unsigned long base_address = find_lib_addr(argv[1]);
  if (base_address == 0) {
    printf("Failed to find addr\n");
    return 1;
  }
  printf("libc.so.6 base address: 0x%lx\n", base_address);
  return 0;
}
