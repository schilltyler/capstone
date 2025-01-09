#include <dirent.h>
#include <stdio.h>

void usage(char *argv_0) { printf("Usage: ./%s <optional path>\n", argv_0); }

int main(int argc, char *argv[]) {
  char *default_dir = ".";
  char *target_dir = NULL;
  switch (argc) {
  case 1:
    target_dir = default_dir;
    break;
  case 2:
    target_dir = argv[1];
    break;
  default:
    usage(argv[0]);
    return 0;
  };
  // //your solution here!
  return (0);
}
