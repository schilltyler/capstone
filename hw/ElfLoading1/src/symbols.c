#include "symbols.h"
#include "utils.h"
#include <dlfcn.h>

// Define RTLD_DEFAULT if not available
#ifndef RTLD_DEFAULT
#define RTLD_DEFAULT ((void *) 0)
#endif

// Problem 3: Symbol Resolution

// Resolve a local symbol (defined within the loaded binary)
uint64_t resolve_local_symbol(Elf64_Sym *sym, uint64_t load_base,
                               uint64_t min_vaddr) {
    // Your solution here!
}

// Resolve an external symbol (from a shared library)
void *resolve_external_symbol(const char *symbol_name, void **dep_handles,
                               int num_deps) {
    // Your solution here!
}

// Resolve any symbol (dispatches to local or external resolver)
uint64_t resolve_symbol(Elf64_Sym *sym, const char *symbol_name,
                        DynInfo *info) {
    // Your solution here!
}

// Print symbol table for debugging
void print_symbol_table(Elf64_Sym *symtab, size_t count, const char *strtab) {
    // Your solution here!
}
