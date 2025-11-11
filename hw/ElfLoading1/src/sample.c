#include "utils.h"
#include "syscalls.h"

int main(int argc, char **argv) {
    mini_printf("Hello from loaded ELF!\n");
    mini_printf("Program: %s\n", argv[0]);
    mini_printf("Arguments: %d\n", argc);

    // Print some memory addresses to show we're running
    mini_printf("Stack variable address: %p\n", &argc);
    mini_printf("Function address: %p\n", (void *)main);

    mini_printf("Successfully executed!\n");
    return 0;
}
