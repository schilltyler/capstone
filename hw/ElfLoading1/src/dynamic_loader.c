#include "mini_loader.h"
#include "dynamic.h"
#include "reloc.h"
#include "symbols.h"
#include "deps.h"
#include "init.h"
#include "utils.h"
#include "syscalls.h"

// Problem 6: Dynamic Loader Integration

void load_dynamic_elf(const char *path) {
    // Your solution here!
}

int main(int argc, char **argv) {
    if (argc != 2) {
        mini_printf("Usage: %s <elf_file>\n", argv[0]);
        return 1;
    }

    load_dynamic_elf(argv[1]);

    // Should never reach here
    return 0;
}
