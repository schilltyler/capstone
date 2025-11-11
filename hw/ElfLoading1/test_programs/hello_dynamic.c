#include <stdio.h>

// Test 1: Simple dynamically linked program
// Tests: Basic relocation, libc linking, printf resolution

int main() {
    printf("Hello from dynamic loader!\n");
    printf("This program is dynamically linked against libc.\n");
    return 0;
}
