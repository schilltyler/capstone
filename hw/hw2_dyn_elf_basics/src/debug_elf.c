#include "debug_elf.h"
#ifdef DEBUG_ELF

#else
// When DEBUG is not defined, make these functions no-ops.
#define debug_esym(sym) ((void)0)
#define debug_shdr(shdr) ((void)0)
#define debug_phdr(phdr) ((void)0)
#define debug_ehdr(ehdr) ((void)0)
#endif
