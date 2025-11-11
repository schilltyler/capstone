#include "reloc.h"
#include "symbols.h"
#include "utils.h"

// Problem 2: Implement Relocations

// Problem 2a: Process R_AARCH64_RELATIVE relocations
// Formula: *target = load_base + r_addend
int process_relative_relocations(DynInfo *info) {
    // Your solution here!
}

// Problem 2b: Process R_AARCH64_GLOB_DAT relocations (GOT)
// Formula: *target = symbol_value + r_addend
int process_glob_dat_relocations(DynInfo *info) {
    // Your solution here!
}

// Problem 2c: Process R_AARCH64_JUMP_SLOT relocations (PLT)
// Formula: *target = symbol_value + r_addend
int process_jump_slot_relocations(DynInfo *info) {
    // Your solution here!
}

// Process all relocations
int process_all_relocations(DynInfo *info) {
    // Your solution here!
}

// Print relocations for debugging
void print_relocations(Elf64_Rela *rela, size_t count,
                      Elf64_Sym *symtab, const char *strtab) {
    // Your solution here!
}

// Verify relocations were applied correctly
int verify_relocations(DynInfo *info) {
    // Your solution here!
}
