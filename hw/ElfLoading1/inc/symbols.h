#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "elf_format.h"
#include "dynamic.h"
#include <stdint.h>  // For uint64_t

// Symbol resolution functions

// Resolve a symbol that is defined locally (within the loaded binary)
// Parameters:
//   sym: The symbol table entry
//   load_base: Base address where the binary is loaded
//   min_vaddr: Minimum virtual address from PT_LOAD segments
// Returns: Actual address of the symbol in memory
uint64_t resolve_local_symbol(Elf64_Sym *sym, uint64_t load_base,
                               uint64_t min_vaddr);

// Resolve an external symbol (from a shared library dependency)
// Parameters:
//   symbol_name: Name of the symbol to resolve
//   dep_handles: Array of dlopen handles for loaded dependencies
//   num_deps: Number of dependencies
// Returns: Address of the symbol, or NULL if not found
void *resolve_external_symbol(const char *symbol_name, void **dep_handles,
                               int num_deps);

// Resolve any symbol (checks if local or external)
// This is a convenience function that delegates to the appropriate resolver
uint64_t resolve_symbol(Elf64_Sym *sym, const char *symbol_name,
                        DynInfo *info);

// Debug helper: print symbol table
void print_symbol_table(Elf64_Sym *symtab, size_t count, const char *strtab);

#endif /* SYMBOLS_H */
