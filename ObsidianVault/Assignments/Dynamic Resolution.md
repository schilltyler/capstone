---
title: Dynamic Resolution
release: 2024-02-10
due: 2025-02-24
tags:
  - release
  - due
ctf: NA
---
# Dynamic Symbol Resolution and ELF  internals

This assignment is divided into several parts that will guide you through exploring dynamic linking, and parsing ELF files. You will implement several utilities and programs that resolve symbols at runtime and print debugging information about ELF binaries.

> **READ THE FRIENDLY MANUAL:**
> 
> - The Friendly Manual
> - `man dlopen`
> - `man dlsym`
> - `man dlinfo`
> - `man procfs`
> - `man elf`

---

## Part 1: Dynamic Symbol Resolution with `ld.so`

In this part you will use the standard dynamic loading functions.

1. **Load `libc.so.6`:**
    
    - Use `dlopen` to load the shared library `libc.so.6`.
    - **In a comment:** Explain what the return value represents.  
        _Hint:_ Is it a virtual address?
2. **Resolve the `printf` Symbol:**
    
    - Use `dlsym` to retrieve the virtual address of the `printf` function.
    - **Restriction:** You are not allowed to declare `printf` as a symbol in your program. Instead, define a function pointer type (for example, `typedef int (*my_printf)(const char *, ...);`), cast the return value of `dlsym` to that type, and call it.
3. **Identify the Base Address of `libc.so.6`:**
    
    - Use one of the `dl*` family of functions to to obtain the virtual base address where `libc.so.6` is loaded.

---

## Part 2: Dynamic Symbol Resolution Using `procfs` and ELF Parsing

In this part, you will accomplish the same goal as Part 1—but **without** using any dynamic loading functions (no `dlopen`, `dlsym`, ...etc).

### Step 1: Find the Base Address Using `/proc/self/maps`

- **Read the Process Maps:**
    - Open and read the file `/proc/self/maps`.
    - Inspect the file (e.g., by running `cat /proc/self/maps`) to see how memory mappings are listed.
- **Locate `libc.so.6`:**
    - Find the line corresponding to `libc.so.6`.  
        _Example output:_
        
        ```bash
        cat /proc/self/maps | grep libc.so.6
        7fff61800000-7fff61988000 r-xp 00000000 103:02 5543  /usr/lib/aarch64-linux-gnu/libc.so.6
        ```
        
    - The first hexadecimal number (`0x7fff61800000` in this example) is the base address of `libc.so.6`.
    - _Tip:_ Use functions like `sscanf()` to parse each line.

### Step 2: Parse the ELF Structure to Find `printf`

4. **Understand the ELF Format:**
    
    - Review the ELF header, program headers, and dynamic segment entries (such as `DT_STRTAB` and `DT_SYMTAB`).  
        _Hint:_ Check `man elf` for details.
5. **Locate the Dynamic Segment:**
    
    - Starting at the base address from Step 1, access the ELF header and then iterate over the program headers.
    - Identify the program header that describes the dynamic segment (`PT_DYNAMIC`).
6. **Extract the Symbol and String Tables:**
    
    - From the dynamic segment, retrieve the addresses of:
        - `DT_STRTAB`: the string table.
        - `DT_SYMTAB`: the symbol table.
    - You may also need the size (e.g., from `DT_STRSZ`).
7. **Find the `printf` Symbol:**
    
    - Walk through the symbol table entries (`Elf64_Sym` structures).
    - For each symbol:
        - Use the `st_name` field (an offset into the string table) to get the symbol’s name.
        - Compare the name with `"printf"`.
        - **Sanity Check:** Ensure the symbol’s type (stored in the lower bits of `st_info`) is `STT_FUNC`.
            - The layout of `st_info` is:
                
                ```
                Bit Positions:      7    6    5    4    3    2    1    0
                                 +----+----+----+----+----+----+----+----+
                                 | B7 | B6 | B5 | B4 | T3 | T2 | T1 | T0 |
                                 +----+----+----+----+----+----+----+----+
                                    Binding (STB_*)    Type (STT_*)
                ```
                
8. **Compute the Virtual Address of `printf`:**
    
    - Add the symbol’s `st_value` (its offset in `libc`) to the base address of `libc.so.6`. This gives you the virtual address of `printf`.
9. **Invoke `printf`:**
    
    - Cast the computed address to an appropriate function pointer and call it.  
        _For example:_ Use your earlier defined function pointer type to call `printf` and have it print its own address.

---

## Part 3: ELF Debugging Utility (DebugElf)

Implement a set of utilities to print detailed information about ELF binaries.

10. **Implement Debug Functions:**
    
    - Write functions to print the following ELF structures:
        - `Elf64_Ehdr` (ELF header)
        - `Elf64_Phdr` (program header)
        - `Elf64_Shdr` (section header)
        - `Elf64_Sym` (symbol table entry)
    - Each function should print the type name and all fields, each on a new line and indented by two spaces. For example:
        
        ```c
        void debug_ehdr(const Elf64_Ehdr *ehdr) {
            debug_printf("Elf64_Ehdr:\n");
            debug_printf("  e_entry: %#llx\n", ehdr->e_entry);
            // ... print other fields ...
        }
        ```
        
11. **Use the Following Table for Types and Format Specifiers:**

| ELF Type      | C Type        | Format Specifier  | Description                         |
| ------------- | ------------- | ----------------- | ----------------------------------- |
| Elf64_Addr    | uint64_t      | `%llx` or `%#llx` | Unsigned program address (hex)      |
| Elf64_Off     | uint64_t      | `%llu` or `%#llx` | Unsigned file offset                |
| Elf64_Section | uint16_t      | `%hu`             | Unsigned section index              |
| Elf64_Versym  | uint16_t      | `%hu`             | Unsigned version symbol information |
| Elf_Byte      | unsigned char | `%02x`            | 8-bit byte (hex)                    |
| Elf64_Half    | uint16_t      | `%hu` or `%#hx`   | 16-bit half                         |
| Elf64_Sword   | int32_t       | `%d`              | 32-bit signed integer               |
| Elf64_Word    | uint32_t      | `%u` or `%#x`     | 32-bit unsigned integer             |
| Elf64_Sxword  | int64_t       | `%lld`            | 64-bit signed integer               |
| Elf64_Xword   | uint64_t      | `%llu` or `%#llx` | 64-bit unsigned integer             |
    
12. **Header File:**
    
    - Provide a header (e.g., `debug_elf.h`) with prototypes for these debug functions. When debugging is not enabled (e.g., when `DEBUG_ELF` is not defined), these functions should compile to no‑ops.

---

## Part 4: Dynamic Resolution Without `dl` Functions (Revisited)

Using the work from Part 2, re‑implement the dynamic resolution of `printf` without using any `dl*` functions.

13. **Recap the Steps:**
    
    - Find the base address of `libc.so.6` using `/proc/self/maps`.
    - Parse the ELF header and program headers to locate the dynamic segment.
    - Retrieve `DT_STRTAB` and `DT_SYMTAB` from the dynamic segment.
    - Walk the symbol table, compare symbol names to `"printf"`, and ensure the symbol type is `STT_FUNC`.
    - Calculate the virtual address of `printf` by adding its `st_value` to the base address.
    - Invoke `printf` (via a cast function pointer) to print its own address.
14. **Restrictions:**
    
    - **Do Not Use:** Any functions from the `dl*` family (i.e., no `dlopen`, `dlsym`, or `dlinfo`).

---

## Challenge: Direct Syscall Implementation

Re-implement the dynamic resolution from Part 4 **using only direct syscalls** (such as `mmap`, `openat`, `read`, etc.), avoiding any libc functions (or any library for that matter!)


- **Requirements:**
    
    - Parse `/proc/self/maps` using syscalls.
    - Implement your own versions of common functions (e.g., `strcmp`, `strstr`, `memcpy`) as needed.
- **Tip:**
    
    - To help with parsing hexadecimal numbers, you might write a helper function:
        
        ```c
        uint64_t hex_str_to_u64(const char *s) {
            uint64_t val = 0;
            // Your implementation here.
            return val;
        }
        ```
        
- **Note:**
    
    - Since your binary might not normally load `libc.so.6` if it doesn’t depend on libc, run your program with:
        
        ```bash
        LD_PRELOAD=/usr/lib/aarch64-linux-gnu/libc.so.6 ./my_program
        ```
        
    - This ensures that `libc.so.6` is present in the process.

---

## Extra Challenge: Assembly-Only Implementation

For the extra challenge, re-implement the dynamic resolution entirely in assembly. The final output should be position‑independent shellcode that locates `libc.so.6` in memory, finds the address of `printf`, and (for example) prints its own address.

