#include "elf_debug.h"
#include "syscalls.h"
#include "utils.h"

#define PAGE_SIZE 0x1000
#define PAGE_ALIGN_DOWN(x) ((x) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(x) (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

// Read entire ELF file into memory
int read_elf_file(const char *path, void **out_data, size_t *out_size) {
    // Your solution here!
}

// Free ELF file data
void free_elf_file(void *data, size_t size) {
    if (data && size > 0) {
        sys_munmap(data, size);
    }
}

// Parse and validate ELF header
int parse_elf_header(const void *data, size_t size, Elf64_Ehdr **out_ehdr) {
    // Your solution here!
}

// Print ELF header information
void print_elf_header(const Elf64_Ehdr *ehdr) {
    // Your solution here!
}

// Get program headers from ELF data
Elf64_Phdr *get_program_headers(const void *elf_data, const Elf64_Ehdr *ehdr) {
    return (Elf64_Phdr *)((uint8_t *)elf_data + ehdr->e_phoff);
}

// Print program headers table
void print_program_headers(const Elf64_Phdr *phdr_table, int phnum) {
    // Your solution here!
}

// Print segment memory mapping information
void print_segments(const void *elf_data, const Elf64_Ehdr *ehdr) {
    // Your solution here!
}

// Validate ELF at virtual address
int validate_elf_at_address(uintptr_t addr) {
    // Your solution here!
}
