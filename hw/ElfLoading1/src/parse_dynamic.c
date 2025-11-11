#include "dynamic.h"
#include "utils.h"
#include "syscalls.h"

// Problem 1: Parse Dynamic Section

// Find the PT_DYNAMIC segment from program headers
Elf64_Dyn *find_dynamic_section(void *elf_base, Elf64_Ehdr *ehdr) {
    // Your solution here!
}

// Parse the dynamic section and populate DynInfo structure
int parse_dynamic_section(Elf64_Dyn *dynamic, DynInfo *info,
                         void *load_base, uint64_t min_vaddr) {
    // Your solution here!
}

// Print dynamic section for debugging
void print_dynamic_section(Elf64_Dyn *dynamic, const char *strtab) {
    // Your solution here!
}

// Free dynamically allocated memory in DynInfo
void free_dyninfo(DynInfo *info) {
    // Your solution here!
}
