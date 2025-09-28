#include <stdio.h>

#define MIN_STR_LEN 4

// FindStr takes a buffer cwith the loaded file, and its size
// It then prints all null terminiated ASCII range strings
void find_str(char *buffer, int buffer_size) {
  char ascii_min = 32;
  char ascii_max = 126;
  // //your solution here!
}

// Loads The file contents, calculates the size, then calls FindStr
void my_strings(char *fileName) {
  // GetFileSize
  int fileSize;
  char *buffer;

  // //your solution here!
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 0;
  }
  my_strings(argv[1]);
  return 0;
}
