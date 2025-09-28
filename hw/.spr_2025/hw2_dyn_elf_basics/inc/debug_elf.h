#include "debug.h"

#ifdef DEBUG_ELF
#include <elf.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

// Print an Elf64_Sym.
void debug_esym(const Elf64_Sym *sym);

// Print an Elf64_Shdr.
void debug_shdr(const Elf64_Shdr *shdr);

// Print an Elf64_Phdr.
void debug_phdr(const Elf64_Phdr *phdr);

// Print an Elf64_Ehdr.
void debug_ehdr(const Elf64_Ehdr *ehdr);

#else
// When DEBUG is not defined, make these functions no-ops.
#define debug_esym(sym) ((void)0)
#define debug_shdr(shdr) ((void)0)
#define debug_phdr(phdr) ((void)0)
#define debug_ehdr(ehdr) ((void)0)
#endif
