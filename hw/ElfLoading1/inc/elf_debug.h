#ifndef ELF_DEBUG_H
#define ELF_DEBUG_H

#include "elf_format.h"
#include <stddef.h>

// Debug printf macro - only prints if DEBUG is defined
#ifdef DEBUG
#define DEBUG_PRINTF(...) mini_printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...) do {} while(0)
#endif

// ELF file reading
int read_elf_file(const char *path, void **out_data, size_t *out_size);
void free_elf_file(void *data, size_t size);

// ELF header parsing
int parse_elf_header(const void *data, size_t size, Elf64_Ehdr **out_ehdr);
void print_elf_header(const Elf64_Ehdr *ehdr);

// Program header parsing
Elf64_Phdr *get_program_headers(const void *elf_data, const Elf64_Ehdr *ehdr);
void print_program_headers(const Elf64_Phdr *phdr_table, int phnum);

// Segment analysis
void print_segments(const void *elf_data, const Elf64_Ehdr *ehdr);

// ELF validation at address
int validate_elf_at_address(uintptr_t addr);

#endif /* ELF_DEBUG_H */
