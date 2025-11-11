#include "mini_loader.h"
#include "elf_format.h"
#include "syscalls.h"
#include "utils.h"

#define PAGE_SIZE 0x1000
#define PAGE_ALIGN_DOWN(x) ((x) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(x) (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

void *read_file_into_memory(const char *path, size_t *size) {
    // Your solution here!
}

uintptr_t map_elf(void *elf_data, size_t size) {
    // Your solution here!
}

void load_elf_from_path(const char *path) {
    // Your solution here!
}

int main(int argc, char **argv) {
    if (argc != 2) {
        mini_printf("Usage: %s <elf_file>\n", argv[0]);
        return 1;
    }

    load_elf_from_path(argv[1]);

    // Should never reach here
    return 0;
}
