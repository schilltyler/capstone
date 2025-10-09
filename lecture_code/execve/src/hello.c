#include "syscalls.h"

int main(int argc, char **argv) {
  const char msg[] = "Hello world!\n";
  sys_write(1, msg, sizeof(msg));
  return 0;
}
