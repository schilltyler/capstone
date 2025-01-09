#include <stdio.h>
#include <stdlib.h>

void usage(char *arg_0) {
  printf("usage: ./%s <n>\nn -- Compute the nth Fibonacci number, n>=0");
  return;
}

int Fibonacci(int n) {
  // //your solution here!
}

int main(int argc, char *argv[]) {
  int n;
  if (argc != 2) {
    usage(argv[0]);
    return 0;
  }
  // Parse the value of n here using atoi!
  // //your solution here!

  if (n < 0) {
    usage(argv[0]);
    return 0;
  }
  int result = Fibonacci(n);
  printf("%d\n", result);
  return 0;
}
