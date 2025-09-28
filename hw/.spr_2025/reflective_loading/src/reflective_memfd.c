#define _GNU_SOURCE // Needed for memfd_create constants and potentially others
#include <elf.h>
#include <stdint.h>
#include <sys/mman.h>  // For MFD_CLOEXEC, PROT_*, MAP_*
#include <sys/types.h> // For size_t, etc.

// Required for constants used in find_symbol_symtab / ELF parsing
#include <string.h> // We will replace these with our own implementations

// --- You will need implementations or headers for these ---
#include "procfsutil.h" // Assumed to declare: search_lib_procmaps()
#include "syscall.h" // Assumed to declare: sys_memfd_create(), sys_write(), sys_close() etc.
#include "sysdebug.h" // Optional: For DEBUG_WRITE_LOG()

// --- Stack String Definitions (Optional but used in reference) ---
// These avoid string literals in .rodata, potentially useful for
// obfuscation/packing.
#define STACK_STR_DLOPEN(var)                                                  \
  char var[7];                                                                 \
  var[0] = 'd';                                                                \
  var[1] = 'l';                                                                \
  var[2] = 'o';                                                                \
  var[3] = 'p';                                                                \
  var[4] = 'e';                                                                \
  var[5] = 'n';                                                                \
  var[6] = '\0';
#define STACK_STR_SPRINTF(var)                                                 \
  char var[8];                                                                 \
  var[0] = 's';                                                                \
  var[1] = 'p';                                                                \
  var[2] = 'r';                                                                \
  var[3] = 'i';                                                                \
  var[4] = 'n';                                                                \
  var[5] = 't';                                                                \
  var[6] = 'f';                                                                \
  var[7] = '\0';
#define STACK_STR_LIBCSO(var)                                                  \
  char var[8];                                                                 \
  var[0] = 'l';                                                                \
  var[1] = 'i';                                                                \
  var[2] = 'b';                                                                \
  var[3] = 'c';                                                                \
  var[4] = '.';                                                                \
  var[5] = 's';                                                                \
  var[6] = 'o';                                                                \
  var[7] = '\0';
#define STACK_STR_PROC_FD(var)                                                 \
  char var[17];                                                                \
  var[0] = '/';                                                                \
  var[1] = 'p';                                                                \
  var[2] = 'r';                                                                \
  var[3] = 'o';                                                                \
  var[4] = 'c';                                                                \
  var[5] = '/';                                                                \
  var[6] = 's';                                                                \
  var[7] = 'e';                                                                \
  var[8] = 'l';                                                                \
  var[9] = 'f';                                                                \
  var[10] = '/';                                                               \
  var[11] = 'f';                                                               \
  var[12] = 'd';                                                               \
  var[13] = '/';                                                               \
  var[14] = '%';                                                               \
  var[15] = 'd';                                                               \
  var[16] = '\0';
#define STACK_STR_RXP(var)                                                     \
  char var[5];                                                                 \
  var[0] = 'r';                                                                \
  var[1] = '-';                                                                \
  var[2] = 'x';                                                                \
  var[3] = 'p';                                                                \
  var[4] = '\0';

// --- Function Pointer Types ---
typedef void *(*dlopen_func_t)(const char *filename, int flag);
typedef int (*sprintf_func_t)(char *str, const char *format, ...);
// typedef void *(*dlsym_func_t)(void *handle, const char *symbol); // Not used
// in this specific example, but often needed

// Basic integer to string conversion might be needed if not using sprintf
// Or implement a minimal sprintf if needed for path construction.
// The reference code bootstraps sprintf itself.

// ============================================================================
// ELF Parsing / Searching Helpers (Implement or Reuse)
// ============================================================================

/**
 * @brief Find the base address of the current loader code in memory.
 */
static uint8_t *find_base() {
  // Hint: Take the address of a function (like find_base itself).
  // Align this address down to a page boundary (e.g., & ~0xFFFUL).
  // Scan backwards page by page from there, looking for the ELF magic bytes.
  DEBUG_WRITE_LOG("[?] Finding own base address...\n");
  uint8_t *p = (uint8_t *)&find_base;
  // ... implement search loop ...
  DEBUG_WRITE_LOG("[-] find_base() not implemented yet.\n"); // Placeholder
  return NULL; // Return found base address or NULL
}

/**
 * @brief Parse an ELF's dynamic symbol table to find a specific symbol's
 * address. (Can be reused from previous assignment)
 */
static void *find_symbol_symtab(void *base_addr, const char *symbol_name) {
  // Hint: Reuse implementation from previous "Hard Mode".
  // Parse Ehdr -> Phdr -> PT_DYNAMIC -> Dyn entries (DT_SYMTAB, DT_STRTAB).
  // Iterate through symbols, compare names, return `base_addr + st_value`.
  // --- Basic Structure ---
  if (!base_addr || !symbol_name)
    return NULL;
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)base_addr;
  if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0)
    return NULL; // Basic check
  if (ehdr->e_phoff == 0)
    return NULL;
  Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)base_addr + ehdr->e_phoff);
  Elf64_Dyn *dynamic_section = NULL;
  DEBUG_WRITE_LOG(
      "[-] find_symbol_symtab() not implemented yet.\n"); // Placeholder

  return NULL; // Symbol not found
}

// ============================================================================
// Main MemFD Loader Entry Point
// ============================================================================

/**
 * @brief Loads the current ELF image using memfd_create and dlopen.
 * @param span The size (in bytes) of the current ELF image in memory.
 * This needs to be determined accurately before calling.
 * (The reference code had it passed in, how it gets calculated
 * beforehand isn't shown in that snippet).
 */
void *load_memfd(size_t span) {
  DEBUG_WRITE_LOG("[+] MemFD Loader Started.\n");

  // --- Stage 1: Find Self ---
  DEBUG_WRITE_LOG("[1] Finding own ELF base...\n");
  uint8_t *self_base = find_base();
  if (!self_base) {
    DEBUG_WRITE_LOG("[-] Failed to find own base address.\n");
    return NULL;
  }

  // --- Stage 2: Bootstrap - Find libc and key functions ---
  uintptr_t libc_base = 0;
  char libc_path[1024] = {0}; // Or use PATH_MAX if defined
  STACK_STR_LIBCSO(libc_str); // "libc.so"
  STACK_STR_RXP(perm);        // "r-xp"

  DEBUG_WRITE_LOG("[2] Finding libc.so.6...\n");
  // You need search_lib_procmaps from procfsutil.h/c
  if (search_lib_procmaps(libc_str, perm, &libc_base, libc_path,
                          sizeof(libc_path)) != 0) {
    DEBUG_WRITE_LOG("[-] Failed to find libc base address.\n");
    return NULL;
  }

  DEBUG_WRITE_LOG("[2] Finding dlopen/sprintf in libc...\n");
  STACK_STR_DLOPEN(str_dlopen);
  STACK_STR_SPRINTF(str_sprintf);

  dlopen_func_t g_dlopen =
      (dlopen_func_t)find_symbol_symtab((void *)libc_base, str_dlopen);
  sprintf_func_t g_sprintf =
      (sprintf_func_t)find_symbol_symtab((void *)libc_base, str_sprintf);

  if (!g_dlopen || !g_sprintf) {
    DEBUG_WRITE_LOG("[-] Failed to find dlopen or sprintf.\n");
    // Decide if you can continue without sprintf, maybe build path manually?
    return NULL;
  }

  // --- Stage 3: Create memfd and write self into it ---
  DEBUG_WRITE_LOG("[3] Creating memfd...\n");
  // You need sys_memfd_create from syscall.h/c
  // Use a dummy name, MFD_CLOEXEC is good practice.
  int mem_fd = sys_memfd_create("elf_mem", MFD_CLOEXEC);
  if (mem_fd < 0) {
    DEBUG_WRITE_LOG("[-] Failed to create memfd.\n");
    return NULL;
  }

  // You need sys_write from syscall.h/c
  ssize_t bytes_written = sys_write(mem_fd, self_base, span);
  if (bytes_written < 0 || (size_t)bytes_written < span) {
    DEBUG_WRITE_LOG("[-] Failed to write complete ELF to memfd.\n");
    // optionally implement sys_close
    return NULL;
  }

  // --- Stage 4: Construct path and call dlopen ---
  char proc_path[256];              // Buffer for /proc/self/fd/%d path
  STACK_STR_PROC_FD(fd_format_str); // "/proc/self/fd/%d"

  DEBUG_WRITE_LOG("[4] Constructing path...\n");
  // Use the bootstrapped sprintf function pointer!
  g_sprintf(proc_path, fd_format_str, mem_fd);

  DEBUG_WRITE_LOG("[4] Calling dlopen on memfd path...\n");
  // Use the bootstrapped dlopen function pointer!
  // RTLD_NOW forces relocation resolution immediately. RTLD_LAZY defers it.
  void *handle = g_dlopen(proc_path, RTLD_NOW | RTLD_LOCAL);

  if (!handle) {
    DEBUG_WRITE_LOG("[-] dlopen failed on memfd path.\n");
    // dlerror() would normally give details, but we can't easily call that yet.
    return NULL;
  }

  DEBUG_WRITE_LOG("[+] The ELF image should now be loaded by ld.so.\n");

  // What to return? The handle from dlopen? Or just NULL signalling completion?
  // The reference code returned NULL. If you wanted to interact with the newly
  // loaded instance, you'd need the handle and dlsym.
  return handle; // Return the handle, might be more useful than NULL
}
