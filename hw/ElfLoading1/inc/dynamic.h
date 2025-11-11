#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "elf_format.h"
#include <stddef.h>  // For size_t

// Dynamic information structure
// This holds all the parsed data from the PT_DYNAMIC segment
typedef struct {
    void *dynstr;           // Dynamic string table (.dynstr)
    void *dynsym;           // Dynamic symbol table (.dynsym)
    Elf64_Rela *rela_dyn;   // .rela.dyn relocations (GOT, etc.)
    Elf64_Rela *rela_plt;   // .rela.plt relocations (PLT)
    size_t rela_dyn_count;  // Number of .rela.dyn entries
    size_t rela_plt_count;  // Number of .rela.plt entries
    uint64_t load_base;     // Base address where segments are loaded
    uint64_t min_vaddr;     // Minimum virtual address from PT_LOAD
    Elf64_Addr *init_array; // Initialization function array (.init_array)
    size_t init_array_count; // Number of init functions
    void (*init_func)(void); // DT_INIT function (legacy)
    char **needed;          // Array of dependency library names
    size_t needed_count;    // Number of dependencies
    void **dep_handles;     // dlopen handles for dependencies
} DynInfo;

// Function declarations for dynamic section parsing
Elf64_Dyn *find_dynamic_section(void *elf_base, Elf64_Ehdr *ehdr);
int parse_dynamic_section(Elf64_Dyn *dynamic, DynInfo *info,
                         void *load_base, uint64_t min_vaddr);
void print_dynamic_section(Elf64_Dyn *dynamic, const char *strtab);
void free_dyninfo(DynInfo *info);

#endif /* DYNAMIC_H */
