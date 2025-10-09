
#include "syscalls.h"
#include "utils.h"

int main(int argc, char **argv) {
  const char msg[] = "Hello world!\n";
  const char greet[] = "and also to you ";
  sys_write(1, msg, sizeof(msg));
  for (int i = 1; i < argc; i++) {
    sys_write(1, greet, sizeof(greet));
    sys_write(1, argv[i], strlen(argv[i]));
    sys_write(1, "\n", 1);
  }
  return 0;
}
