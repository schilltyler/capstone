#include <stdio.h>
#include <string.h>

#define KEY "\\ch0nky.txt"

int activate() {
  // //your solution here!
  return 0;
}

int deactivate() {
  // //your solution here!
  return 0;
}

void usage(char *argv_0) { printf("Usage: %s <activate|deactivate>", argv_0); }
int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv[0]);
    return 0;
  }
  if (strcmp(argv[1], "activate") == 0) {
    activate();
  } else if (strcmp(argv[1], "deactivate") == 0) {
    deactivate();
  } else {
    usage(argv[0]);
  }

  return 0;
}
