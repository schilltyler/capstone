#ifndef RELOC_H
#define RELOC_H

#include "elf_format.h"
#include "dynamic.h"

// AArch64 Relocation Types
// These are the three main relocation types you'll handle in this assignment

// R_AARCH64_RELATIVE: Base relative relocation
// Formula: *target = load_base + r_addend
// Used for: Position-independent code, adjusting internal pointers
#define R_AARCH64_RELATIVE  1027  // 0x403

// R_AARCH64_GLOB_DAT: GOT entry for global data
// Formula: *target = symbol_value + r_addend
// Used for: Global variables from shared libraries
#define R_AARCH64_GLOB_DAT  1025  // 0x401

// R_AARCH64_JUMP_SLOT: PLT entry for function calls
// Formula: *target = symbol_value + r_addend
// Used for: Function calls to shared libraries
#define R_AARCH64_JUMP_SLOT 1026  // 0x402

// Additional relocation types (for reference, not required in this assignment)
#define R_AARCH64_ABS64     257   // 0x101
#define R_AARCH64_COPY      1024  // 0x400
#define R_AARCH64_TLSDESC   1031  // 0x407

// Relocation processing functions
int process_relative_relocations(DynInfo *info);
int process_glob_dat_relocations(DynInfo *info);
int process_jump_slot_relocations(DynInfo *info);
int process_all_relocations(DynInfo *info);

// Debug helpers
void print_relocations(Elf64_Rela *rela, size_t count,
                      Elf64_Sym *symtab, const char *strtab);
int verify_relocations(DynInfo *info);

#endif /* RELOC_H */
