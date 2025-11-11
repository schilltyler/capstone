#include "elf_debug.h"
#include "syscalls.h"
#include "utils.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        mini_printf("Usage: %s <elf_file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    // Your solution here!

    return 0;
}
