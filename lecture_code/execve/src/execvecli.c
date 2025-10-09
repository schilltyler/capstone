#include "syscalls.h"
#include <stddef.h>
int main(int argc, char **argv) {
  if (argc < 2) {
    const char usage[] = "Usage: execveli <path> <argv>\n";
    sys_write(2, usage, sizeof(usage));
    return 1;
  }
  char **new_argv = NULL;
  char **envp = NULL;
  if (argc > 2) {
    new_argv = &argv[2];
  }
  long ret = sys_execve(argv[1], argv, envp);

  sys_exit(1);
  return 0;
}
