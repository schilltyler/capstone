#include <stdio.h>

int main() {
  int x = 0xdeadbeef;
  unsigned char *y = (unsigned char *)&x;
  for (int i = 0; i < 4; i++) {
    unsigned char c = y[i] & 0xff;
    printf("%x ", c);
  }
  printf("\n");
}
