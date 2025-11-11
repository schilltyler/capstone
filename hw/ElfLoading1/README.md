# ElfLoading1 Assignment Plan - Dynamic Linking and Shared Objects

## Overview

This assignment builds on ElfLoading0 by extending the minimal ELF loader to support **dynamic linking**. Students will implement:

1. **GOT (Global Offset Table) relocations** via `.rela.dyn`
2. **PLT (Procedure Linkage Table) relocations** via `.rela.plt`
3. **Dependency loading** using `DT_NEEDED` entries
4. **Initialization arrays** (`.init_array`)
5. **Shared object loading** with symbol resolution

---

By completing this assignment, students will:

- Understand the dynamic linking process in Linux
- Learn how GOT and PLT enable position-independent code
- Implement relocation processing for AArch64
- Handle symbol resolution across multiple loaded objects
- Work with the dynamic section and understand `DT_*` entries
- Execute initialization functions in the correct order
- Build a functional dynamic loader from first principles

---

## Prerequisites

Students must have completed **ElfLoading0** and have working implementations of:

- ELF header parsing
- Program header processing
- PT_LOAD segment mapping with page alignment
- Load bias calculation for PIE executables
- Basic memory management (mmap, mprotect)
- Entry point execution

**Starting Point**: Students receive their completed ElfLoading0 code as the starter codebase.

---

## Assignment Structure

### Problem Set

#### Problem 1: Parse Dynamic Section (15 points)

**File**: `src/parse_dynamic.c`

Implement functions to parse the PT_DYNAMIC segment and extract critical dynamic linking information.

**Requirements:**

- Locate PT_DYNAMIC segment from program headers
- Parse all dynamic entries (`Elf64_Dyn` structures)
- Extract and store:
  - `DT_SYMTAB`: Dynamic symbol table address
  - `DT_STRTAB`: Dynamic string table address
  - `DT_RELA`: Relocation table address (`.rela.dyn`)
  - `DT_RELASZ`: Size of relocation table
  - `DT_RELAENT`: Size of each relocation entry
  - `DT_JMPREL`: PLT relocation table address (`.rela.plt`)
  - `DT_PLTRELSZ`: Size of PLT relocations
  - `DT_NEEDED`: List of dependencies
  - `DT_INIT_ARRAY`: Initialization function array
  - `DT_INIT_ARRAYSZ`: Size of init array
  - `DT_INIT`: Single initialization function

**Functions to implement:**

```c
// Find PT_DYNAMIC segment
Elf64_Dyn *find_dynamic_section(void *elf_base, Elf64_Ehdr *ehdr);

// Parse dynamic section and populate DynInfo structure
int parse_dynamic_section(Elf64_Dyn *dynamic, DynInfo *info,
                         void *load_base, uint64_t min_vaddr);

// Print dynamic section for debugging
void print_dynamic_section(Elf64_Dyn *dynamic, const char *strtab);
```

**Provided Structure:**

```c
typedef struct {
    void *dynstr;           // Dynamic string table
    void *dynsym;           // Dynamic symbol table
    Elf64_Rela *rela_dyn;   // .rela.dyn relocations
    Elf64_Rela *rela_plt;   // .rela.plt relocations
    size_t rela_dyn_count;  // Number of .rela.dyn entries
    size_t rela_plt_count;  // Number of .rela.plt entries
    uint64_t load_base;     // Base address of loaded segments
    Elf64_Addr *init_array; // Initialization function pointers
    size_t init_array_size; // Number of init functions
    void (*init_func)(void); // DT_INIT function
    char **needed;          // Array of dependency names
    size_t needed_count;    // Number of dependencies
} DynInfo;
```

---

#### Problem 2: Implement Relocations (25 points)

**File**: `src/relocate.c`

Process relocations to fix up addresses in the loaded binary.

**Sub-problem 2a: R_AARCH64_RELATIVE (5 points)**

Handle position-independent code relocations.

```c
// Relocation formula: *target = load_base + addend
int process_relative_relocations(DynInfo *info);
```

**Requirements:**

- Iterate through `.rela.dyn` entries
- For each `R_AARCH64_RELATIVE` relocation:
  - Calculate target address: `target = load_base + r_offset`
  - Write new value: `*target = load_base + r_addend`
- Handle both `.rela.dyn` and `.rela.plt` sections

**Sub-problem 2b: R_AARCH64_GLOB_DAT (10 points)**

Handle GOT (Global Offset Table) relocations for global variables.

```c
// Relocation formula: *target = symbol_value + addend
int process_glob_dat_relocations(DynInfo *info);
```

**Requirements:**

- Extract symbol index from `r_info` using `ELF64_R_SYM()`
- Look up symbol in dynamic symbol table
- Resolve symbol address:
  - If symbol is defined locally (st_shndx != SHN_UNDEF): use `load_base + st_value`
  - If symbol is external (st_shndx == SHN_UNDEF): use `dlsym()` to resolve
- Write GOT entry: `*target = symbol_value + r_addend`

**Sub-problem 2c: R_AARCH64_JUMP_SLOT (10 points)**

Handle PLT (Procedure Linkage Table) relocations for function calls.

```c
// Relocation formula: *target = symbol_value + addend
int process_jump_slot_relocations(DynInfo *info);
```

**Requirements:**

- Similar to GLOB_DAT but processes `.rela.plt` section
- Resolve function symbols from external libraries
- Populate PLT entries with actual function addresses
- This implements **eager binding** (all symbols resolved at load time)

**Relocation Type Constants (provided):**

```c
#define R_AARCH64_RELATIVE  1027  // 0x403
#define R_AARCH64_GLOB_DAT  1025  // 0x401
#define R_AARCH64_JUMP_SLOT 1026  // 0x402
```

---

#### Problem 3: Symbol Resolution (20 points)

**File**: `src/symbols.c`

Implement symbol lookup and resolution across loaded objects.

**Requirements:**

**Function 1: Local Symbol Resolution (10 points)**

```c
uint64_t resolve_local_symbol(Elf64_Sym *sym, uint64_t load_base,
                               uint64_t min_vaddr);
```

- Check if symbol is defined (st_shndx != SHN_UNDEF)
- Calculate actual address: `symbol_addr = load_base + (st_value - min_vaddr)`
- Handle special section indices (SHN_ABS, SHN_COMMON)

**Function 2: External Symbol Resolution (10 points)**

```c
void *resolve_external_symbol(const char *symbol_name, void **dep_handles,
                               int num_deps);
```

- Search through loaded dependencies using `dlsym()`
- Try each dependency handle in order
- Fall back to `RTLD_DEFAULT` for system libraries
- Return NULL if symbol not found (report error)

**Symbol Table Structure:**

```c
// Elf64_Sym fields:
// - st_name: Offset into string table
// - st_value: Symbol value (address or offset)
// - st_shndx: Section index (SHN_UNDEF = undefined)
// - st_info: Type and binding (STT_FUNC, STT_OBJECT, etc.)
```

---

#### Problem 4: Load Dependencies (20 points)

**File**: `src/load_deps.c`

Load shared libraries specified by `DT_NEEDED` entries.

**Requirements:**

**Function: Load All Dependencies**

```c
int load_dependencies(DynInfo *info, void ***handles_out);
```

**Implementation Steps:**

1. Iterate through `DT_NEEDED` entries from dynamic section
2. Extract library name from string table: `libname = dynstr + d_un.d_val`
3. Load each dependency using `dlopen(libname, RTLD_LAZY | RTLD_GLOBAL)`
4. Store handles in array for later symbol resolution
5. Handle errors gracefully (report which dependency failed)
6. Return number of successfully loaded dependencies

**Common Dependencies:**

- `libc.so.6`: C standard library
- `libm.so.6`: Math library
- `libpthread.so.0`: POSIX threads
- Custom shared libraries

**Important Considerations:**

- Load order matters (dependencies may depend on each other)
- Use `RTLD_GLOBAL` to make symbols available for subsequent loads
- Dependencies are loaded BEFORE relocations are processed
- System linker handles transitive dependencies automatically

---

#### Problem 5: Initialization Functions (10 points)

**File**: `src/init.c`

Execute initialization functions after loading and relocating.

**Requirements:**

**Function 1: Execute Init Array**

```c
void call_init_array(DynInfo *info);
```

- Locate `.init_array` from `DT_INIT_ARRAY` and `DT_INIT_ARRAYSZ`
- Calculate number of function pointers: `count = size / sizeof(void*)`
- Call each function in order: `init_array[i]()`
- Handle NULL pointers safely

**Function 2: Execute Init Function**

```c
void call_init_function(DynInfo *info);
```

- Check if `DT_INIT` is present
- Cast address to function pointer: `void (*init)(void) = (void (*)(void))addr`
- Call the initialization function

**Execution Order:**

1. Load all dependencies first
2. Process all relocations
3. Call dependency init functions (recursive)
4. Call main binary's `DT_INIT` (if present)
5. Call main binary's `.init_array` (if present)
6. Finally jump to entry point

---

#### Problem 6: Dynamic Loader Integration (10 points)

**File**: `src/dynamic_loader.c`

Integrate all components into a complete dynamic loader.

**Requirements:**

Update `load_elf_from_path()` from ElfLoading0 to:

```c
void load_dynamic_elf(const char *path) {
    // 1. Load ELF file (from ElfLoading0)
    void *elf_data = read_file_into_memory(path, &size);

    // 2. Map segments (from ElfLoading0)
    LoadedElf *elf = map_elf_segments(elf_data, size);

    // 3. Find and parse dynamic section (NEW)
    Elf64_Dyn *dynamic = find_dynamic_section(elf->base, elf->ehdr);
    DynInfo info = {0};
    parse_dynamic_section(dynamic, &info, elf->base, elf->min_vaddr);

    // 4. Load dependencies (NEW)
    void **dep_handles;
    int num_deps = load_dependencies(&info, &dep_handles);

    // 5. Process relocations (NEW)
    process_relative_relocations(&info);
    process_glob_dat_relocations(&info);
    process_jump_slot_relocations(&info);

    // 6. Execute initialization (NEW)
    call_init_function(&info);
    call_init_array(&info);

    // 7. Jump to entry point
    void (*entry)(void) = (void (*)(void))elf->entry;
    entry();
}
```

---

## Test Programs

### Test 1: Simple Dynamic Binary

**File**: `test_programs/hello_dynamic.c`

```c
#include <stdio.h>

int main() {
    printf("Hello from dynamic loader!\n");
    return 0;
}
```

Compile: `gcc -o hello_dynamic hello_dynamic.c`

- Tests: Basic relocation, libc linking, printf resolution

### Test 2: Math Library Usage

**File**: `test_programs/use_libm.c`

```c
#include <stdio.h>
#include <math.h>

int main() {
    printf("sqrt(2.0) = %f\n", sqrt(2.0));
    printf("sin(PI/2) = %f\n", sin(M_PI/2));
    return 0;
}
```

Compile: `gcc -o use_libm use_libm.c -lm`

- Tests: Multiple dependencies (libc + libm), GOT relocations for data

### Test 3: Init Array

**File**: `test_programs/init_test.c`

```c
#include <stdio.h>

__attribute__((constructor))
void init1() {
    printf("Init function 1 called\n");
}

__attribute__((constructor))
void init2() {
    printf("Init function 2 called\n");
}

int main() {
    printf("Main function called\n");
    return 0;
}
```

Compile: `gcc -o init_test init_test.c`

- Tests: `.init_array` execution order

### Test 4: Custom Shared Library

**File**: `test_programs/mylib.c`

```c
#include <stdio.h>

void my_function() {
    printf("Custom library function called!\n");
}

int my_global_var = 42;
```

**File**: `test_programs/use_mylib.c`

```c
#include <stdio.h>

extern void my_function();
extern int my_global_var;

int main() {
    printf("Global var = %d\n", my_global_var);
    my_function();
    return 0;
}
```

Compile:

```bash
gcc -shared -fPIC -o libmylib.so mylib.c
gcc -o use_mylib use_mylib.c -L. -lmylib -Wl,-rpath=.
```

- Tests: Custom dependencies, GOT for data, PLT for functions

---

## Debugging Tools

### Tool 1: Print Relocations

```c
void print_relocations(Elf64_Rela *rela, size_t count,
                      Elf64_Sym *symtab, const char *strtab);
```

Display all relocations with:

- Relocation type name
- Symbol name
- Target address
- Addend value

### Tool 2: Print Symbol Table

```c
void print_symbol_table(Elf64_Sym *symtab, size_t count, const char *strtab);
```

Display:

- Symbol name
- Symbol value
- Section index
- Type (FUNC, OBJECT, etc.)

### Tool 3: Verify Relocations

```c
int verify_relocations(DynInfo *info);
```

Check that all relocations were applied correctly:

- No NULL GOT entries for required symbols
- All PLT entries point to valid code
- Relative relocations point within loaded segments

---

## Grading Breakdown (100 points)

| Component | Points | Description |
|-----------|--------|-------------|
| **Build Success** | 5 | All programs compile without errors |
| **Problem 1: Parse Dynamic** | 15 | Correctly extracts all DT_* entries |
| **Problem 2a: RELATIVE** | 5 | Position-independent relocations work |
| **Problem 2b: GLOB_DAT** | 10 | GOT relocations for global data |
| **Problem 2c: JUMP_SLOT** | 10 | PLT relocations for functions |
| **Problem 3: Symbol Resolution** | 20 | Local and external symbols resolve |
| **Problem 4: Dependencies** | 20 | DT_NEEDED libraries load correctly |
| **Problem 5: Initialization** | 10 | Init array executes in order |
| **Problem 6: Integration** | 5 | Complete loader works end-to-end |
| **Testing** | 10 | All test programs execute correctly |

**Total: 100 points**

### Test Program Points Breakdown

- hello_dynamic: 2 points
- use_libm: 3 points
- init_test: 2 points
- use_mylib: 3 points

---

## Implementation Strategy

### Phase 1: Understanding (Before coding)

1. Read the ELF specification on dynamic sections
2. Study the AArch64 relocation types
3. Understand the GOT/PLT mechanism
4. Review provided header structures

### Phase 2: Parsing (Problem 1)

1. Implement dynamic section parsing
2. Create debug print functions
3. Verify against `readelf -d` output

### Phase 3: Basic Relocations (Problem 2a)

1. Start with R_AARCH64_RELATIVE (simplest)
2. Test with position-independent code
3. Verify addresses are correct

### Phase 4: Symbol Resolution (Problem 3)

1. Implement local symbol lookup
2. Add external symbol resolution with dlsym
3. Test symbol table access

### Phase 5: Advanced Relocations (Problem 2b, 2c)

1. Implement GLOB_DAT for GOT
2. Implement JUMP_SLOT for PLT
3. Test with libc functions (printf, malloc, etc.)

### Phase 6: Dependencies (Problem 4)

1. Parse DT_NEEDED entries
2. Load libraries with dlopen
3. Handle errors gracefully

### Phase 7: Initialization (Problem 5)

1. Implement init_array execution
2. Add DT_INIT support
3. Verify execution order

### Phase 8: Integration and Testing (Problem 6)

1. Combine all components
2. Test with all provided test programs
3. Debug issues systematically

---

## Provided Infrastructure

### Headers

- All headers from ElfLoading0
- New: `dynamic.h` - Dynamic section structures and constants
- New: `reloc.h` - Relocation type definitions
- Updated: `mini_loader.h` - New function declarations

### Source Files (from ElfLoading0)

- `start.S` - Entry point
- `utils.c` - Utility functions
- `elf_utils.c` - ELF parsing utilities
- `mini_loader.c` - Basic segment loading

### New Skeleton Files (to implement)

- `parse_dynamic.c` - Dynamic section parsing
- `relocate.c` - Relocation processing
- `symbols.c` - Symbol resolution
- `load_deps.c` - Dependency loading
- `init.c` - Initialization functions
- `dynamic_loader.c` - Main loader integration

### Debugging Support

- Enhanced `mini_printf` with hex formatting
- Memory dump utilities
- Relocation tracing (can be enabled/disabled)

---

## Concepts

### Global Offset Table (GOT)

- **Purpose**: Indirect access to global variables
- **Why**: Enables position-independent data access
- **How**: Code references GOT entry, GOT entry contains actual address
- **Relocation**: Loader fills GOT entries with resolved addresses

### Procedure Linkage Table (PLT)

- **Purpose**: Indirect function calls
- **Why**: Enables position-independent function calls
- **How**: Code calls PLT stub, PLT jumps to address in GOT
- **Relocation**: Loader fills GOT entries with function addresses

### Load Bias

- **Definition**: Difference between file virtual address and actual load address
- **Calculation**: `load_bias = actual_base - min_vaddr`
- **Usage**: All addresses must be adjusted by load bias
- **Critical**: Entry point, symbol values, relocation offsets

### Symbol Binding

- **Eager**: All symbols resolved at load time (this assignment)
- **Lazy**: Symbols resolved on first use (LD_BIND_NOW=0)
- **Trade-off**: Eager is slower to load but faster to execute

---

## Resources

### Required Reading

1. ELF Specification - Dynamic Linking section
2. AArch64 ELF ABI - Relocation types
3. `man elf` - ELF structure reference
4. `man dlopen` - Dynamic loading API

### Useful Commands

```bash
# View dynamic section
readelf -d program

# View relocations
readelf -r program

# View symbol table
readelf -s program

# View dependencies
ldd program

# Trace dynamic linker
LD_DEBUG=all ./program

# View GOT/PLT sections
objdump -d program | grep -A20 '<.plt>'
```

### Example Readelf Output Analysis

```bash
# Dynamic section entries
readelf -d bin/hello_dynamic

# Expected output:
# Tag        Type                         Name/Value
# DT_NEEDED  Shared library              [libc.so.6]
# DT_RELA    0x400400                    # Relocation table
# DT_RELASZ  96 (bytes)                  # Total size
# DT_SYMTAB  0x400300                    # Symbol table
# DT_STRTAB  0x400350                    # String table
# DT_JMPREL  0x400450                    # PLT relocations
# DT_PLTRELSZ 48 (bytes)                 # PLT size
```

---

## Common Pitfalls

### 1: Incorrect Address Calculation

**Problem**: Segfault when accessing relocated address
**Cause**: Forgetting to apply load bias
**Solution**: Always use `load_base + (vaddr - min_vaddr)`

### 2: Wrong Relocation Type

**Problem**: GOT entries contain garbage values
**Cause**: Processing JUMP_SLOT as GLOB_DAT or vice versa
**Solution**: Check relocation type carefully, use switch statement

### 3: Unresolved Symbols

**Problem**: NULL pointer in GOT entry causes crash
**Cause**: Symbol not found in dependencies
**Solution**: Check dlsym return value, verify dependency loaded

### 4: Init Array Not Called

**Problem**: Constructors don't run
**Cause**: Calling entry point before init_array
**Solution**: Execute init functions BEFORE jumping to entry

### 5: Dependency Order

**Problem**: Symbol resolution fails
**Cause**: Loading dependencies in wrong order
**Solution**: Load dependencies in the order they appear in DT_NEEDED
