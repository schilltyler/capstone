#ifndef MINI_LOADER_H
#define MINI_LOADER_H

#include <stdint.h>
#include <stddef.h>

// Read an entire file into memory
// Returns pointer to file contents, sets *size to file size in bytes
// Returns NULL on failure
void *read_file_into_memory(const char *path, size_t *size);

// Map ELF segments into memory with correct permissions
// Returns entry point address, or 0 on failure
uintptr_t map_elf(void *elf_data, size_t size);

// Load and execute an ELF file from path
// This function should not return (it jumps to the loaded program)
void load_elf_from_path(const char *path);

#endif /* MINI_LOADER_H */
