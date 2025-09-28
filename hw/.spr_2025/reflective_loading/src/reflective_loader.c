#define _GNU_SOURCE
#include <elf.h>
#include <stdint.h>
#include <sys/mman.h>  // For protection flags (PROT_*) and mmap flags (MAP_*)
#include <sys/types.h> // For size_t, etc.

// --- You will need implementations or headers for these ---
#include "procfsutil.h" // Assumed to declare: search_lib_procmaps()
#include "syscall.h" // Assumed to declare: sys_mmap(), sys_mprotect(), maybe others
#include "sysdebug.h" // Optional: For DEBUG_WRITE_LOG()

// --- Standard Definitions ---
#define MAX_HANDLES 256
// #define PATH_MAX // Usually defined in <limits.h>, but we can't include that
// yet. Define if needed.

// Function pointer types
typedef void *(*dlopen_func_t)(const char *filename, int flag);
typedef void *(*dlsym_func_t)(void *handle, const char *symbol);
#ifdef DEBUG
// Only needed if you bootstrap printf for debugging
typedef int (*printf_func_t)(const char *format, ...);
#endif

// Context structures
struct lib_handle {
  void *handle;
  // Maybe add lib name for debugging?
};

struct dyn_loader_ctx {
  dlopen_func_t dlopen;
  dlsym_func_t dlsym;
#ifdef DEBUG
  printf_func_t printf;
#endif
  struct lib_handle handles[MAX_HANDLES];
  int handle_count; // Keep track of loaded library handles
};

struct elf_ctx {
  const void *base_addr; // Base address where this ELF is loaded *in the target
                         // mapping*
  uintptr_t load_bias; // Difference between base_addr and ELF's preferred load
                       // address (phdr[0].p_vaddr)

  Elf64_Ehdr *ehdr;
  Elf64_Phdr *phdr;
  // Section headers (shdr) are often stripped, rely on dynamic section

  Elf64_Dyn *dynamic; // Pointer to the dynamic section

  // Pointers populated from dynamic section (adjust by load_bias)
  Elf64_Sym *dynsym;
  char *dynstr;
  size_t dynstrsz;
  size_t syment; // Size of a symbol entry

  Elf64_Rela *rela; // .rela.dyn
  size_t relasz;
  size_t relaent;

  Elf64_Rel *rel; // .rel.dyn (less common on AArch64)
  size_t relsz;
  size_t relent;

  void *jmprel; // .rela.plt or .rel.plt
  size_t pltrelsz;
  Elf64_Word pltrel; // Type (DT_RELA or DT_REL)

  void (*init)(void);
  void (*fini)(void);
  void **init_array;
  size_t init_arraysz;
  // fini_array might also be needed if implementing unloading

  // Other potentially useful fields from dynamic section (hash, pltgot, soname,
  // etc.)
};

// ============================================================================
// ELF Parsing and Loading Helpers (Implement These)
// ============================================================================

/**
 * @brief Verify basic ELF64 sanity checks.
 */
static inline int verify_elf64(const void *base_addr) {
  // Hint: Check the first 4 bytes for ELF magic (0x7F, 'E', 'L', 'F').
  // Check e_ident[EI_CLASS] == ELFCLASS64.
  // Return 0 if valid, -1 otherwise.
  DEBUG_WRITE_LOG("[?] Verifying ELF header...\n");
  if (!base_addr)
    return -1;
  const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *)base_addr;
  // ... implement checks ...
  // // TODO:.....
  if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || /* ... other magic bytes ... */
      ehdr->e_ident[EI_CLASS] != ELFCLASS64) {
    DEBUG_WRITE_LOG("[-] ELF verification failed.\n");
    return -1;
  }
  DEBUG_WRITE_LOG("[+] ELF header looks OK.\n");
  return 0;
}

/**
 * @brief Find the base address of the current loader code in memory.
 * Needed to know where the ELF data is to copy/map from.
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
 * address. Used primarily to find dlopen/dlsym within libc initially.
 */
static void *find_symbol_symtab(void *base_addr, const char *symbol_name) {
  // Hint:
  // 1. Get ELF Header (Ehdr) from base_addr.
  // 2. Get Program Headers (Phdr) using ehdr->e_phoff.
  // 3. Find the PT_DYNAMIC segment in Phdr.
  // 4. Iterate through the dynamic section (Elf64_Dyn*) pointed to by
  // PT_DYNAMIC's p_vaddr (+ bias if needed).
  // 5. Find DT_SYMTAB (symbol table) and DT_STRTAB (string table) entries. Get
  // their addresses (d_un.d_ptr).
  // 6. Iterate through the symbol table (Elf64_Sym*). For each symbol:
  //    a. Get the symbol name: `name = string_table_addr + symbol->st_name`.
  //    b. Compare `name` with `symbol_name` using your `strcmp`.
  //    c. If it matches and the symbol is suitable (e.g., defined function),
  //    return `base_addr + symbol->st_value`.
  // Remember that addresses in the dynamic section might need adjusting by a
  // load bias depending on how the library was loaded. For libc found via
  // procmaps, base_addr *is* the load bias.
  DEBUG_WRITE_LOG("[?] Finding symbol in symtab (not implemented)...\n");
  return NULL; // Return symbol address or NULL
}

/**
 * @brief Map the PT_LOAD segments of an ELF file into a new memory region.
 */
static void *mmap_elf_segments(uint8_t *elf_start) {
  // Hint:
  // 1. Get Ehdr and Phdr from elf_start.
  // 2. Iterate through Phdr *once* to find the minimum p_vaddr and maximum
  // (p_vaddr + p_memsz) across all PT_LOAD segments. Align min down and max up
  // to page boundaries. Calculate total size.
  // 3. Call `sys_mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE |
  // MAP_ANONYMOUS, -1, 0);` to allocate target memory. This is `target_base`.
  // Check for errors (MAP_FAILED).
  // 4. Iterate through Phdr *again* for PT_LOAD segments:
  //    a. Calculate destination address: `dest = target_base + (phdr[i].p_vaddr
  //    - min_vaddr)`. b. Copy `phdr[i].p_filesz` bytes from `elf_start +
  //    phdr[i].p_offset` to `dest` using your `memcpy`. c. If `phdr[i].p_memsz
  //    > phdr[i].p_filesz`, zero out the difference at `dest +
  //    phdr[i].p_filesz` using your `memset` (this handles the .bss section).
  // 5. Optionally (or do it later): Iterate through Phdr *again* for PT_LOAD
  // segments and use `sys_mprotect` to set the correct final memory permissions
  // (PROT_READ, PROT_WRITE, PROT_EXEC) based on `phdr[i].p_flags`. Initially
  // mapping RW is simpler for relocations.
  // 6. Return `target_base`.
  DEBUG_WRITE_LOG("[?] Mapping ELF segments (not implemented)...\n");
  return NULL; // Return target_base or NULL
}

/**
 * @brief Parse the dynamic section of a mapped ELF file.
 */
static int process_elf_ctx(const void *base_addr, struct elf_ctx *ctx) {
  // Hint:
  // 1. Zero out the ctx struct using your `memset`.
  // 2. Set `ctx->base_addr`. Verify ELF header using `verify_elf64`.
  // 3. Get Ehdr and Phdr pointers.
  // 4. Calculate the `ctx->load_bias`. Find the *first* PT_LOAD segment
  // (usually p_offset == 0). `load_bias = (uintptr_t)base_addr - ph->p_vaddr`.
  // 5. Find the PT_DYNAMIC segment in Phdr. Set `ctx->dynamic` to its location
  // in memory (`base_addr + ph->p_vaddr` if ET_EXEC, potentially adjusted by
  // bias or offset for ET_DYN).
  // 6. Iterate through the dynamic section entries (`Elf64_Dyn *dyn =
  // ctx->dynamic; dyn->d_tag != DT_NULL; dyn++`).
  // 7. Inside the loop, use a `switch(dyn->d_tag)`:
  //    a. For entries like DT_STRTAB, DT_SYMTAB, DT_RELA, DT_INIT,
  //    DT_INIT_ARRAY, etc. (pointer values):
  //       Calculate the runtime address: `addr = dyn->d_un.d_ptr +
  //       ctx->load_bias`. Store this `addr` in the corresponding `ctx` field
  //       (e.g., `ctx->dynstr = (char*)addr;`).
  //    b. For entries like DT_STRSZ, DT_RELASZ, DT_INIT_ARRAYSZ, etc.
  //    (size/value entries):
  //       Store `dyn->d_un.d_val` directly in the corresponding `ctx` field
  //       (e.g., `ctx->dynstrsz = dyn->d_un.d_val;`).
  // 8. Ensure crucial fields like dynstr, dynsym, rela/jmprel are found if
  // needed.
  DEBUG_WRITE_LOG("[?] Processing ELF context (not implemented)...\n");
  return -1; // Return 0 on success, -1 on error
}

/**
 * @brief Load libraries listed in DT_NEEDED entries.
 */
static int process_needed_libs(struct dyn_loader_ctx *loader_ctx,
                               struct elf_ctx *elf_ctx) {
  // Hint:
  // 1. Check if `elf_ctx->dynamic` and `elf_ctx->dynstr` are valid.
  // 2. Iterate through the dynamic section (`Elf64_Dyn* dyn = elf_ctx->dynamic;
  // ...`).
  // 3. If `dyn->d_tag == DT_NEEDED`:
  //    a. Get the library name: `lib_name = elf_ctx->dynstr + dyn->d_un.d_val`.
  //    b. Call `loader_ctx->dlopen(lib_name, RTLD_LAZY | RTLD_LOCAL)`. (Use the
  //    function pointer!) Check for errors. c. If successful, store the
  //    returned handle in `loader_ctx->handles[loader_ctx->handle_count]` and
  //    increment `handle_count`. Check for MAX_HANDLES overflow.
  DEBUG_WRITE_LOG("[?] Processing needed libraries (not implemented)...\n");
  return -1; // Return 0 on success, -1 on error
}

/**
 * @brief Perform relocations (RELA, REL, PLT).
 */
static int process_relocations(struct elf_ctx *elf_ctx,
                               struct dyn_loader_ctx *loader_ctx) {
  // Hint: This is complex! Focus on RELA (.rela.dyn and .rela.plt) first.
  // AArch64 primarily uses RELA.

  // --- Process .rela.dyn (DT_RELA) ---
  // 1. Check if `elf_ctx->rela` and `elf_ctx->relasz` are valid.
  // 2. Calculate number of entries: `count = elf_ctx->relasz /
  // elf_ctx->relaent`.
  // 3. Loop `i` from 0 to `count`:
  //    a. Get the relocation entry: `Elf64_Rela *rel = &elf_ctx->rela[i];`
  //    b. Calculate target address: `target_addr_ptr =
  //    (Elf64_Addr*)(elf_ctx->base_addr + rel->r_offset);` c. Get relocation
  //    type: `type = ELF64_R_TYPE(rel->r_info);` d. Get symbol index: `sym_idx
  //    = ELF64_R_SYM(rel->r_info);` e. Get addend: `addend = rel->r_addend;` f.
  //    `sym_val = 0;` g. If `sym_idx > 0`:
  //       i. Get the symbol: `Elf64_Sym *sym = &elf_ctx->dynsym[sym_idx];`
  //       ii. If `sym->st_shndx == SHN_UNDEF`: // Undefined symbol, needs
  //       lookup
  //           * Get symbol name: `sym_name = elf_ctx->dynstr + sym->st_name;`
  //           * Loop through `loader_ctx->handles`:
  //              `sym_val = (Elf64_Addr)loader_ctx->dlsym(handle, sym_name);`
  //              (Use function pointer!) If found (`sym_val != 0`), break the
  //              loop.
  //           * If still not found (`sym_val == 0`), it's an error. Return -1.
  //       iii. Else (symbol defined locally):
  //           `sym_val = (Elf64_Addr)elf_ctx->base_addr + sym->st_value;`
  //    h. Apply relocation based on `type`:
  //       * `case R_AARCH64_RELATIVE:` `*target_addr_ptr = elf_ctx->base_addr +
  //       addend;` (B + A)
  //       * `case R_AARCH64_GLOB_DAT:` `*target_addr_ptr = sym_val + addend;`
  //       (S + A)
  //       * `case R_AARCH64_ABS64:`    `*target_addr_ptr = sym_val + addend;`
  //       (S + A)
  //       * // Handle other types if necessary (COPY, TLS etc. are harder)
  //       * `default:` Error, unsupported type. Return -1.

  // --- Process .rela.plt (DT_JMPREL, if DT_PLTREL == DT_RELA) ---
  // 1. Check `elf_ctx->jmprel`, `elf_ctx->pltrelsz`, `elf_ctx->pltrel ==
  // DT_RELA`.
  // 2. Similar loop as above, using `Elf64_Rela *plt_rel =
  // (Elf64_Rela*)elf_ctx->jmprel;`.
  // 3. Look up symbols using `dlsym` as they are typically external function
  // calls.
  // 4. Apply relocation:
  //    * `case R_AARCH64_JUMP_SLOT:` `*target_addr_ptr = sym_val + addend;` (S
  //    + A)
  //    * `default:` Error. Return -1.

  DEBUG_WRITE_LOG("[?] Processing relocations (not implemented)...\n");
  return -1; // Return 0 on success, -1 on error
}

/**
 * @brief Call initialization functions (DT_INIT, DT_INIT_ARRAY).
 */
static int process_init(struct elf_ctx *elf_ctx,
                        struct dyn_loader_ctx *loader_ctx) {
  // Hint:
  // 1. Check if `elf_ctx->init` is not NULL. If so, cast it to `void
  // (*func)(void) = elf_ctx->init;` and call `func();`.
  // 2. Check if `elf_ctx->init_array` and `elf_ctx->init_arraysz` are valid.
  // 3. Calculate number of functions: `count = elf_ctx->init_arraysz /
  // sizeof(void*)`.
  // 4. Loop `i` from 0 to `count`:
  //    a. Get function pointer: `void (*func)(void) = elf_ctx->init_array[i];`
  //    b. If `func` is not NULL, call `func();`.
  DEBUG_WRITE_LOG("[?] Processing initializers (not implemented)...\n");
  return 0; // Assuming success for now, return -1 if errors occur
}

// ============================================================================
// Main Reflective Loader Entry Point
// ============================================================================

void *reflective_loader() {
  DEBUG_WRITE_LOG("[+] Reflective Loader Started.\n");

  // --- Stage 1: Bootstrap - Find libc and key functions ---
  struct dyn_loader_ctx mctx = {0}; // Use stack allocation
  struct dyn_loader_ctx *ctx = &mctx;
  uintptr_t libc_base = 0;
  char libc_path[1024] = {0}; // Define PATH_MAX if needed or use fixed size

  DEBUG_WRITE_LOG("[1] Finding libc.so.6...\n");
  // You need search_lib_procmaps from procfsutil.h/c
  if (search_lib_procmaps("libc.so.6", "r-xp", &libc_base, libc_path,
                          sizeof(libc_path)) != 0) {
    DEBUG_WRITE_LOG("[-] Failed to find libc base address.\n");
    return NULL;
  }

  DEBUG_WRITE_LOG("[1] Finding dlopen/dlsym in libc...\n");
  ctx->dlopen = (dlopen_func_t)find_symbol_symtab((void *)libc_base, "dlopen");
  ctx->dlsym = (dlsym_func_t)find_symbol_symtab((void *)libc_base, "dlsym");
// Optionally find printf for debugging:
#ifdef DEBUG
// ctx->printf = (printf_func_t)find_symbol_symtab((void *)libc_base, "printf");
// DEBUG_CTX(ctx, "[+] printf found at %p\n", ctx->printf); // Use DEBUG_CTX
// macro if defined
#endif

  if (!ctx->dlopen || !ctx->dlsym) {
    DEBUG_WRITE_LOG("[-] Failed to find dlopen or dlsym.\n");
    return NULL;
  }

  // --- Stage 2: Find self and map segments ---
  DEBUG_WRITE_LOG("[2] Finding own ELF base...\n");
  uint8_t *self_elf_base_in_memory = find_base();
  if (!self_elf_base_in_memory) {
    DEBUG_WRITE_LOG("[-] Failed to find own base.\n");
    return NULL;
  }

  DEBUG_WRITE_LOG("[2] Mapping own ELF segments into new memory...\n");
  void *target_base = mmap_elf_segments(self_elf_base_in_memory);
  if (!target_base) {
    DEBUG_WRITE_LOG("[-] Failed to map segments.\n");
    return NULL;
  }

  // --- Stage 3: Process the *newly mapped* ELF image ---
  struct elf_ctx ectx = {0}; // ELF context for the *new* mapping
  DEBUG_WRITE_LOG("[3] Processing ELF context for new mapping...\n");
  if (process_elf_ctx(target_base, &ectx) != 0) {
    DEBUG_WRITE_LOG("[-] Failed to process ELF context.\n");
    // Consider unmapping target_base before returning
    return NULL;
  }

  // --- Stage 4: Load Dependencies ---
  DEBUG_WRITE_LOG("[4] Loading needed libraries...\n");
  if (process_needed_libs(ctx, &ectx) != 0) {
    DEBUG_WRITE_LOG("[-] Failed to load dependencies.\n");
    // Consider unmapping target_base
    return NULL;
  }

  // --- Stage 5: Perform Relocations ---
  DEBUG_WRITE_LOG("[5] Performing relocations...\n");
  if (process_relocations(&ectx, ctx) != 0) {
    DEBUG_WRITE_LOG("[-] Failed during relocations.\n");
    // Consider unmapping target_base
    return NULL;
  }

  // --- Stage 6: Call Initializers ---
  DEBUG_WRITE_LOG("[6] Calling initializers...\n");
  if (process_init(&ectx, ctx) != 0) {
    DEBUG_WRITE_LOG("[-] Failed during initialization.\n");
    // Consider unmapping target_base
    return NULL;
  }

  // --- Done ---

  // The `tester.c` will eventually munmap the *original* mapping this code ran
  // from. The new mapping at `target_base` will persist (unless explicitly
  // unmapped later). In a real scenario, you might return a specific function
  // pointer from the new mapping, or the loader itself might jump to the new
  // image's actual entry point if it were an executable.
  return target_base;
}
