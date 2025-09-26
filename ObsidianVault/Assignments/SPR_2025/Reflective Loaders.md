Reflective Payload 


**Task:**
This assignment aims to guide you through building  shellcode payloads  using C code. To get started you will create a small C program designed to be loaded directly into executable memory and run without relying on the standard system dynamic linker/loader (ld.so). 
To accomplish this,  you can still write code in c, but with its grammar restricted. Further, certain compiler flags are needed to help ensure that the code can be run without properly mapping the segments. 

**Background:**

Typically, when you run a program or use a shared library (`.so` file), the operating system's dynamic linker (`ld.so` ) handles loading the code into memory, resolving symbol dependencies, and performing relocations. However, it's possible to write c code that is compiled into a binary that can be run without the help of ld.so. This goes *beyond* just building a static executable, as they often times still need the kernel to memory map the segments.  

To achieve this, the code must be:

1. **Position-Independent (PIC):** It cannot rely on absolute memory addresses, as it doesn't know where it will be loaded. Access to data and functions must use relative addressing. The `-fPIC` flag helps achieve this.
2. **Self-Contained:** Since `ld.so` isn't involved, the code cannot easily link against standard libraries like `libc` unless specifically handled. Flags like `-nostdlib`, `-nostartfiles` prevent linking against standard libraries and including standard startup code.
3. **Compiled Appropriately:** Specific compiler and linker flags are needed to produce a suitable binary format (often a shared object `.so` file, but compiled in a non-standard way). The `-shared` flag creates an `.so` file, and `-e <symbol>` sets the entry point in the ELF header. While not strictly necessary, `-e <symbol>` makes writing bootstraps, testers...etc  a lot easier. Rather than having to find the specific symbol, you can find the offset of the shellcode from the elf header. 

- You may assume that libc is loaded into the processes virtual adders space, but in general this might not always be the case (i.e. systes like Alpine linux that statically link libc) 

In this section, you will write a simple function in C, compile it as a self-contained, position-independent shared object, and use the provided `tester` program to load and execute it. Note that this is **not** a shellcode payload *yet*. To turn it into one, we will append a bootstrap shellcode that simply jumps to the entry point.

## Example PIC payload
1. **`Makefile` Example:** Demonstrates the necessary compiler (`CC`) and linker (`LD`) flags (`LOADER_FLAGS`) and build steps to create the specialized shared object.
    
    Makefile
    
    ```
    LOADER_FLAGS=-c -Iinc -fPIC -O0 -static -shared -nostdlib -fomit-frame-pointer -fno-stack-protector -fno-asynchronous-unwind-tables -nostartfiles -nostdlib
    bin:
        mkdir -p bin
    
    # build object file
    example_pic.o: bin
        $(CC) $(LOADER_FLAGS) src/example_pic.c \
            -o bin/example_pic.o
    
    # link into shared object
    example_pic: example_pic.o
        $(LD) -shared bin/example_pic.o -e sample -o bin/sample.so
    
    # (You will adapt this for calculator.c/calculator.so)
    ```
    
2. **`example_pic.c`:** A minimal example C file containing a function (`sample`) designed to be compiled with the provided `Makefile`. If the reuslting shared object is copied as is from disk into memory,  and `pc` is set to `base_addr` + `elf_enty`, it will run without issues
    
    C
    
    ```
    #include "sysdebug.h" // Optional header
    #include <sys/types.h>
    
    void *sample(size_t n_iters) {
      for (size_t i = 0; i < n_iters; i++) {
        DEBUG_WRITE_LOG("[+] Look mah! no loader\n");
      }
      return (void *)&sample; // Returns its own address as an example
    }
    ```
    
3. **`tester.c`:** A C program that acts as a custom loader for your compiled `.so` file.
    
Note the call to `sleep`, and the `sys_getpid`   definition in `syscall.h`. This makes it easy to set breakpoints near code you want to debug. i.e. `catch syscall sleep`. 


```c
#include <elf.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) { /* ... usage error ... */ return 1; }
  const char *so_path = argv[1];
  // 1. Open the shared object file
  int fd = open(so_path, O_RDONLY);
  /* ... error checks ... */
  struct stat st;
  /* ... fstat call & error checks ... */
  size_t so_size = st.st_size;

  // 2. Allocate a RWX memory region
  void *mapping = mmap(NULL, so_size, PROT_READ | PROT_WRITE | PROT_EXEC,
					   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  /* ... error checks ... */

  // 3. Read the entire file into the allocated memory
  ssize_t rd = read(fd, mapping, so_size);
  /* ... error checks ... */
  close(fd);

  // 4. Locate the entry point offset from ELF header
  Elf64_Ehdr *eh = (Elf64_Ehdr *)mapping;
  size_t func_offset = eh->e_entry; // Read offset set by linker's -e flag
  /* ... error check if func_offset is 0 ... */

  // 5. Calculate address and jump to the function
  printf("[*] Loader: Jumping to function at offset 0x%zx\n", func_offset);
  uintptr_t (*func_ptr)(size_t) = // Cast address to function pointer
	  (uintptr_t(*)(size_t))((char *)mapping + func_offset);

  sleep(0); // to set a breakpoint right bfore we call the payload

  // Execute the code loaded from the .so file!
  uintptr_t result = func_ptr(so_size); // Call function, pass file size

  printf("[*] Loader: Function returned %p\n", (void *)result);
  printf("Nice\n");

  // 6. Unload memory and exit
  munmap(mapping, so_size);
  printf("[+] Gracefully exiting now!\n");
  return 0;
}
```
    
4. **`sysdebug.h` Contains a `DEBUG_WRITE_LOG` macro used in `example_pic.c`. You may use this for debugging your function if you find it helpful, but it's not required.

**Compiler/Linker Flags (`LOADER_FLAGS` and Linker Command):**

- **`-c`**: Compile only, do not link. Produces `.o` object file. _Necessary_ for the two-stage build process.
- **`-fPIC`**: Generate Position-Independent Code. _CRITICAL_ so the code works regardless of where `mmap` loads it.
- **`-O0`**: Disable optimizations. _Recommended_ for easier debugging and predictable low-level behavior.
- **`-static`**: Primarily a linker flag (prefer static libs). Used here during compile, reinforces the "no dynamic linking" goal alongside `-nostdlib`.
- **`-shared` (Compiler/Linker)**: Hints code is for/creates a shared object (`.so`). _Necessary_ (especially for linker) to produce the desired file format.
- **`-nostdlib`**: Do not link against standard C libraries (`libc`) or use standard runtime. _CRITICAL_ because `tester.c` doesn't provide the standard C environment; the `.so` code must be self-contained.
- **`-fomit-frame-pointer`**: Optimization; don't use a register for the frame pointer. _Optional_, makes code slightly smaller/faster but debugging harder.
- **`-fno-stack-protector`**: Disable stack smashing protection (canaries). _Necessary_ because the required runtime support is missing due to `-nostdlib`/`-nostartfiles`.
- **`-fno-asynchronous-unwind-tables`**: Don't generate exception unwind tables. _Optional_, reduces code size as they aren't needed for simple C.
- **`-nostartfiles`**: Do not link standard startup files (like `crt1.o`). _CRITICAL_ because we don't want to link against libc
- **`-e <symbol>` (Linker only)**: Set the program entry point to the address of `<symbol>`. _CRITICAL_ because `tester.c` reads this specific field (`e_entry`) from the ELF header to know which function address to call.

**How the Tester Loads and Executes Your Code:**

The `tester.c` program performs a manual loading process, bypassing the system's standard dynamic linker (`ld.so`). Here's a breakdown and a diagram:

1. **Open & Size:** It opens your compiled `.so` file (e.g., `calculator.so`) and determines its size.
2. **Allocate Memory:** It asks the kernel for a new region of virtual memory using `mmap`. Crucially, it requests permissions `PROT_READ | PROT_WRITE | PROT_EXEC` (Read, Write, Execute) and flags `MAP_PRIVATE | MAP_ANONYMOUS`. This gives it a "blank slate" of memory that it can write to, read from, and later execute code within.
3. **Copy Bytes:** It reads the _entire raw byte content_ of your `.so` file from disk and copies it directly into the memory region allocated by `mmap`. At this point, the memory region contains an exact byte-for-byte copy of the `calculator.so` file.
4. **Find Entry Point:** Since the memory now contains the file content, the first bytes correspond to the ELF Header. The `tester` casts the start of the memory (`mapping`) to an `Elf64_Ehdr*` pointer and reads the `e_entry` field. This field holds the _offset_ (distance from the start of the file/memory region) of the entry point function. The value in `e_entry` was put there by the linker (`ld`) specifically because you used the `-e calculate_sum` flag when building `calculator.so`.
5. **Calculate Address & Cast:** It calculates the absolute virtual address of your function by adding the base address of the mapped region (`mapping`) to the offset read from the header (`func_offset`). It then casts this raw address into a function pointer (`uintptr_t (*func_ptr)(size_t)`) that matches the signature of your `calculate_sum` function.
6. **Execute:** It calls the function using the function pointer (`func_ptr(so_size)`). This causes the CPU to jump to the address of your `calculate_sum` code (which now resides inside the RWX memory region) and begin executing it.
7. **Cleanup:** After the function returns, `tester.c` uses `munmap` to release the memory region.



## Helper functions for grammar restricted c

In order to make writing your reflective loader more straight forward, it is recommended that you implement the following helper functions.  While not strictly necessary, the only  symbol you **must** implement is `memset`. Otherwise `struct foo  bar={0}` will link to `libc!memset`.


1. Make sure you have the `minstr.h` header file. It contains the necessary function declarations and documentation comments explaining what each function should do.
2. Create a new C source file, for example, `minstr.c`.
3. In `minstr.c`, include the header file:
    
    C
    
    ```
    #include "minstr.h"
    ```
    
4. You will write the implementation for each function listed below inside your `minstr.c` file.
5. **Crucially:** _Do not_ look up or copy existing implementations of these standard functions. The goal is to figure out how to implement them yourself based on the descriptions.
6. **Testing:** It is highly recommended to write a separate `main` function (perhaps in another file, e.g., `test.c`) to test each function you implement thoroughly. Test edge cases (e.g., empty strings, null pointers where applicable - though the examples don't handle NULL inputs explicitly, zero lengths, etc.).




### 1. `strstr`
```
char *strstr(const char *haystack, const char *needle);
```

- **Task:** Find the first occurrence of the `needle` string within the `haystack` string.
- **Parameters:**
    - `haystack`: The string to search within.
    - `needle`: The string to search for.
- **Return Value:**
    - If `needle` is found, return a pointer to the beginning of the first occurrence of `needle` within `haystack`.
    - If `needle` is _not_ found, return `NULL`.
    - If `needle` is an empty string, return `haystack`.
- **Hint:** You'll likely need nested loops. The outer loop iterates through possible start positions in `haystack`, and the inner loop compares characters with `needle`.

### 2. `memset`

```
void *memset(void *s, int c, unsigned long n);
```

- **Task:** Fill the first `n` bytes of the memory area pointed to by `s` with the constant byte `c`.
- **Parameters:**
    - `s`: A pointer to the memory area to fill.
    - `c`: The byte value to fill with (note: it's passed as `int` but should be treated as an `unsigned char`).
    - `n`: The number of bytes to fill.
- **Return Value:** Return the original pointer `s`.
- **Hint:** You'll need to cast the `void *` pointer `s` to a character pointer (e.g., `unsigned char *`) to manipulate individual bytes. Iterate `n` times, setting each byte.

### 3. `memcpy`

```
void *memcpy(void *dest, const void *src, unsigned long n);
```

- **Task:** Copy `n` bytes from the memory area pointed to by `src` to the memory area pointed to by `dest`.
- **Parameters:**
    - `dest`: A pointer to the destination memory area.
    - `src`: A pointer to the source memory area.
    - `n`: The number of bytes to copy.
- **Return Value:** Return the original pointer `dest`.
- **Important Note:** You can assume the memory areas (`src` and `dest`) do not overlap. (The standard `memcpy` has undefined behavior if they overlap; `memmove` handles overlap).
- **Hint:** Similar to `memset`, cast the `void *` pointers to character pointers (e.g., `unsigned char *` for `dest` and `const unsigned char *` for `src`) and copy byte by byte in a loop.

### 4. `memcmp`

```
int memcmp(const void *s1, const void *s2, unsigned long n);
```

- **Task:** Compare the first `n` bytes of the memory areas `s1` and `s2`.
- **Parameters:**
    - `s1`: Pointer to the first memory area.
    - `s2`: Pointer to the second memory area.2
    - `n`: The number of bytes to compare.
- **Return Value:**
    - Return an integer less than 0 if the first differing byte in `s1` is less than the corresponding byte in `s2`.
    - Return 0 if the first `n` bytes of `s1` and `s2` are identical.
    - Return an integer greater than 0 if the first differing byte in `s1` is greater than the corresponding byte in `s2`.
- **Hint:** Cast the pointers to `const unsigned char *`. Iterate `n` times, comparing bytes at each position. Return immediately if a difference is found.

### 5. `strlen`

```
unsigned long strlen(const char *s);
```

- **Task:** Calculate the length of the string `s`. The length is the number of characters _before_ the terminating null character (`\0`).
- **Parameters:**
    - `s`: The null-terminated string.
- **Return Value:** The number of bytes in the string, excluding the null terminator.
- **Hint:** Iterate through the string using a pointer, counting characters until you encounter the null character (`\0`). Pointer arithmetic can be useful here.


### 6. `strcmp`

```
int strcmp(const char *s1, const char *s2);
```

- **Task:** Compare the two null-terminated strings `s1` and `s2` lexicographically.
- **Parameters:**
    - `s1`: The first string.
    - `s2`: The second string.
- **Return Value:**
    - Return an integer less than 0 if `s1` is lexicographically less than `s2`.
    - Return 0 if `s1` is equal to `s2`.
    - Return an integer greater than 0 if `s1` is lexicographically greater than `s2`.
- **Hint:** Iterate through both strings simultaneously, comparing characters at the current position. Stop when you find a mismatch or reach the end of _either_ string. The comparison result is based on the _first_ differing characters. Remember to compare using `unsigned char` values.

### 7. `hex_str_to_u64`

```
uintptr_t hex_str_to_u64(const char *s);
```

- **Task:** Convert a string `s` containing hexadecimal digits (0-9, a-f, A-F) into its corresponding unsigned 64-bit integer value (`uintptr_t` is often used like `unsigned long long` or similar).
- **Parameters:**
    - `s`: The string representation of the hexadecimal number.
- **Return Value:** The calculated unsigned 64-bit integer value.
- **Behavior:**
    - The conversion should stop at the first character that is not a valid hexadecimal digit.
    - Handle both lowercase ('a'-'f') and uppercase ('A'-'F') letters.
- **Hint:** Iterate through the string. For each valid hex digit, update the result. You can multiply the current result by 16 (or use a left shift `<< 4`) and then add the value of the new digit. Convert character digits ('0'-'9') and letters ('a'-'f', 'A'-'F') to their numerical values (0-15).




---

# Implement `search_lib_procmaps`

The next helper function you need to implement is  `search_lib_procmaps`. This function will search through the current process's memory map (`/proc/self/maps`) to find a specific loaded shared library (e.g., `libc.so.6`) that has certain memory permissions (e.g., readable and executable). If found, the function should return the starting (base) address of that library's memory segment and optionally its full path.

**Function Signature:**

```
int search_lib_procmaps(char *lib_needle, char *prot,
                        uintptr_t *base_out, char *lib_path,
                        size_t path_max);
```

**Background: `/proc/self/maps`**

On Linux systems, `/proc/self/maps` is a special file that shows how memory is mapped for the currently running process. Each line in this file represents a contiguous region of virtual memory and contains information like:

- **Address Range:** The start and end addresses of the memory region (e.g., `7f45a2c00000-7f45a2e00000`). These are hexadecimal numbers.
- **Permissions:** Flags indicating whether the memory is readable (`r`), writable (`w`), executable (`x`), and shared (`s`) or private (`p`) (e.g., `r-xp`).
- **Offset:** The offset into the file that this mapping corresponds to.
- **Device:** The major/minor device number where the file resides.
- **Inode:** The inode number of the file.
- **Pathname:** The file associated with this memory mapping (if any). This is often the path to an executable or a shared library.

**Example Line:**

```
7f45a2c00000-7f45a2e00000 r-xp 00000000 103:02 1234567  /usr/lib/x86_64-linux-gnu/libc-2.31.so
```

Your function needs to read this file, parse relevant lines, and find one that matches the criteria.

**Parameters:**

- `lib_needle` (Input): A string containing the name (or part of the name) of the library you are searching for (e.g., `"libc.so"`).
- `prot` (Input): A string representing the exact memory permissions required (e.g., `"r-xp"` for read-execute-private).
- `base_out` (Output): A pointer to a `uintptr_t`. If the library is found with the correct permissions, you should store its _starting_ virtual address here.
- `lib_path` (Output, Optional): A pointer to a character buffer. If this pointer is _not_ `NULL` and the library is found, you should copy the full pathname from the `/proc/self/maps` entry into this buffer.
- `path_max` (Input): The maximum number of bytes that can be written into the `lib_path` buffer (including the null terminator). Ignored if `lib_path` is `NULL`.

**Return Value:**

- Return `0` if a matching library entry is found and the base address (and optionally path) has been successfully stored.
- Return `-1` if an error occurs (e.g., cannot open `/proc/self/maps`, cannot read from it) or if no matching entry is found after searching the entire file.

**Implementation Steps (Guidance):**

1. **Open `/proc/self/maps`:**
    
    - Use a system call (like `sys_openat` or the standard `open`) to open the file `/proc/self/maps` for reading (`O_RDONLY`).
    - Handle potential errors (e.g., if the file cannot be opened, return -1). Remember the path: `/proc/self/maps`.
2. **Read File Content:**
    
    - Read the contents of the file into a sufficiently large  stack allocated buffer (e.g., 4096 bytes). Often times, the maps file  will be larger than this. You might need multiple  iterations.
    - Use a system call ( ie `sys_read`) to read the contents of maps.
    - Check for read errors (return -1 if reading fails or returns 0 bytes before finding anything).
    - If you are feeling lazy, you can call sys read 1 character at a time until you get to a `\n`
3. **Process Buffer Line by Line:**
    - Iterate through the buffer, identifying individual lines (lines end with a newline character `\n`).
    - For each line:
        - **Parse the Line:** Extract the necessary information:
            - The starting address (the first part of the address range, before the `-`).
            - The permissions string (immediately after the address range).
            - The pathname (usually the last field on the line, starting after the inode number).
            - _Hint:_ You can find these fields by searching for spaces () and the hyphen (`-`).   The helper functions implemented previously might come in handy here :-)
        - **Check Permissions:** Compare the permissions string you extracted from the line with the `prot` parameter string. Use a memory comparison function (like `memcmp`) for an exact match.
        - **Check Pathname:** If the permissions match, check if the `lib_needle` string exists as a substring within the pathname you extracted from the line. Use a string searching function (like `strstr`).
        - **Match Found:** If _both_ permissions and the pathname match:
            - **Extract Base Address:** Convert the hexadecimal starting address string (e.g., `"7f45a2c00000"`) into an unsigned integer (`uintptr_t`). You might need a helper function for this (like the `hex_str_to_u64` you may have written previously).
            - **Store Base Address:** Store the converted address in the location pointed to by `base_out`.
            - **Store Path (Optional):** If `lib_path` is not `NULL`:
                - Copy the pathname string you extracted into the `lib_path` buffer.
                - Be careful _not_ to write more than `path_max - 1` characters.
                - Ensure the copied string in `lib_path` is null-terminated.
            - **Success:** You've found the match. Close the file descriptor and return `0`.
4. **No Match Found:** If you reach the end of the buffer (or file) without finding a match, close the file descriptor.
    
5. **Close File:** Ensure you close the file descriptor (using `sys_close` or `close`) before returning, whether you found a match or not (unless an error occurred _before_ opening the file).
    
6. **Return Failure:** If no match was found after checking all lines, return `-1`.
    

**Hints and Considerations:**

- Search for common libraries like `"libc.so"`, `"ld-linux"`, or another library you know is loaded.
- Use correct permission strings like `"r-xp"` or `"r--p"`.
- Test both cases where `lib_path` is `NULL` and where it points to a valid buffer.
- Test with a `path_max` value smaller than the actual path length to ensure truncation works.
- Test searching for a library that isn't loaded to ensure it returns -1 correctly.

# Implement a Reflective Memfd Loader


Next, you will write a simple reflective loader using `memfd_create`. The loader should be able to:

1. Find its own image (the code itself) in memory.
2. Create an anonymous in-memory file descriptor using `memfd_create`.
3. Copy its own memory image into this memfd.
	1. To do this, you must know where your payload starts in memory
	2. how large the payload is. This is because you must call `sys_write(memfd_fd, start_addr, size)`
		1. The example tester shows how to pass a parameter to the reflective loader but it is also relatively simple to calculate the size of the elf in at runtime. Recall that earlier in the semester, we  learned how to write static loaders by simply memory mapping the segments. To do this, we needed to know where each portion of the segment started and ended  in the file bytes configuration.  there are other ways to find the size at runtime though. Be creative :-). More on this below
4. Dynamically load the code from the memfd using `dlopen`, effectively loading a copy of itself from memory without needing a separate file on the filesystem at load time.
- **`memfd_create`:** A Linux system call that creates an anonymous file descriptor residing only in RAM. It doesn't correspond to any file on a filesystem but can be written to, read from, and importantly, executed from (e.g., via `dlopen` using its `/proc` path).
- **`/proc/self/fd/`:** A special directory in the `/proc` filesystem where each entry (`<fd_number>`) is a symbolic link to the actual file/resource opened on that file descriptor number. `dlopen` can use paths like `/proc/self/fd/NN` to load libraries.
- **Manual Symbol Resolution:** Finding the address of a function (like `dlopen`) within a loaded library (like `libc.so`) by parsing its ELF structures in memory, rather than relying on the standard `dlsym` function (which might not be available or desirable to link against directly).

**Core Functions to Implement:**

You will likely need to implement several helper functions leading up to the main loader function. Consider structuring your code around these concepts:

1. **`find_own_elf_base()`:**
    
    - **Goal:** Determine the starting virtual address (base address) of the currently executing code's ELF image in memory.
    - **Strategy:**
        - Get the address of a known point within your code (e.g., the address of this `find_own_elf_base` function itself).
        - Align this address down to the nearest page boundary (memory is typically mapped in page-sized chunks, e.g., 4096 bytes).
        - Search backwards from this aligned address, checking page by page for the ELF magic number (`\x7FELF`). The address where the magic number is found is the base address of your code's image.
    - **Return:** The base address (`uint8_t*` or `void*`) or `NULL` on failure.
2. **`get_own_elf_size(void *base)`:** (Optional but Recommended)
    
    - **Goal:** Determine the total memory span occupied by your own ELF image.
    - **Strategy:**
      ### 0
        - Requires parsing the ELF headers located at the `base` address found previously.
        - Access the ELF header (`Elf64_Ehdr`) to find the offset and number of Program Headers (`e_phoff`, `e_phnum`).
        - Iterate through the Program Headers (`Elf64_Phdr`) and note the following
```c
    // offset into the elf where the segment starts
    uint64_t seg_offset = phdrs[i].p_offset; // physical
    uint64_t seg_vaddr = phdrs[i].p_vaddr; // virtual
     // size of the segment 
	uint64_t filesz = phdrs[i].p_filesz; // physical
    uint64_t memsz = phdrs[i].p_memsz; // virtual

// HINT: find the segment with the largest seg_offset
// then compute elf_size = ( elf_start[seg_offset] + filesz) - elf_base
// This is the most reliable way to compute the size of your elf
// asusming properly fomred Program headers
// the only reason this is not the "prefered" method is it increases
// the size of the elf payload.  we could just as easily search for magic 
// bytes without additional parsing logic 
```
### 1
- pass the size of the elf file as an argument to the function. this is easy, as the size can be compute offline (ie. `stat file`). for the bootstrap shellcode, you can set `x0` to the size of the elf. 
### 2
Recommended approach: calculate the "approximate" elf size at run time. 
To accomplish this,  define a symbol after your reflective loader in the source code. compute size_t as `&dummy_func - elf_base`. While not exact, it will  include enough of the elf.  You can also  add some magic bytes to the end of the elf (i.e. magic bytes) and you can increment memory until you find them. In order for this to work, you must define the symbol after the loader function, and also use a linker script to set the  location of the symbol at the bottom of your elf. Depending on your payload, you might also be able to use the `_end_bss` symbol which points to the end of the bss section, which is usally at the end of the elf. Note this will clip the segments below it, so do make sure to run 
`readelf -S rp.so` to ensure you aren't missing anything.
	
3. **`find_symbol_in_library(void *lib_base, const char *symbol_name)`:**
    
    - **Goal:** Find the absolute virtual address of a function (symbol) within another already loaded shared library (like `libc.so`), given that library's base address. This mimics `dlsym`.
    - **Strategy:**
        - Verify the ELF magic number at `lib_base`.
        - Parse the ELF header (`Elf64_Ehdr`) at `lib_base` to find the Program Header Table offset (`e_phoff`).
        - Iterate through Program Headers (`Elf64_Phdr`) to find the one with `p_type == PT_DYNAMIC`. Its virtual address (`p_vaddr`) added to `lib_base` gives the location of the dynamic section.
        - Iterate through the dynamic section entries (`Elf64_Dyn`) until `d_tag == DT_NULL`. Look for entries with tags:
            - `DT_SYMTAB`: Contains the address of the symbol table (`d_un.d_ptr`).
            - `DT_STRTAB`: Contains the address of the string table (`d_un.d_ptr`).
            - `DT_SYMENT` (optional): Size of each symbol table entry.
            - `DT_STRSZ` (optional): Total size of the string table.
        - Once you have the symbol table (`Elf64_Sym *`) and string table (`char *`), iterate through the symbol table.
        - For each symbol entry (`Elf64_Sym`), get the symbol name by adding the name offset (`sym->st_name`) to the string table address.
        - Compare this name with `symbol_name` using `strcmp`.
        - If the names match, the symbol's absolute address is `lib_base + sym->st_value`. Return this address.
    - **Return:** The symbol's address (`void*`) or `NULL` if not found or an error occurs.
4. **`reflective_memfd_load()` (Main Loader Function):**
    
    - **Goal:** Orchestrate the reflective loading process.
    - **Steps:**
        - **Find Self:** Call `find_own_elf_base()` to get the base address of your code. Handle errors.
        - **Determine Size:** Determine the size (`span`) of your code image. You might implement `get_own_elf_size()` or use a predetermined/passed-in size.
        - **Find Libc:** Use the `search_lib_procmaps` function (which you implemented previously!) to find the base address of `libc.so` (e.g., search for `"libc.so"` with `"r-xp"` permissions). Handle errors.
        - **Find `dlopen`:** Call `find_symbol_in_library()`, passing it the `libc_base` and the string `"dlopen"`. Store the returned function pointer in a suitable type (`dlopen_func_t`). Handle errors (if `dlopen` can't be found, loading is impossible).
        - **(Optional) Find `sprintf`:** You might also need to find `"sprintf"` or similar using `find_symbol_in_library` if you plan to construct the `/proc` path dynamically and want to avoid linking `libc` conventionally.
        - **Create Memfd:** Call the `sys_memfd_create` system call (or `memfd_create` from `libc` if preferred). Provide a name (e.g., "myloader") and flags (e.g., `MFD_CLOEXEC`). Check the returned file descriptor; handle errors (return value < 0).
        - **Write to Memfd:** Use `sys_write` (or `write`) to copy your code's image (from `your_base_address` for `span` bytes) into the `mem_fd`. Check for errors.
        - **Construct Proc Path:** Create the path string `/proc/self/fd/<mem_fd>`, replacing `<mem_fd>` with the actual file descriptor number. Use `sprintf` (either the one you found or the standard one) or manual string construction.
        - **Load from Memfd:** Call the `dlopen` function pointer you obtained earlier. Pass it the `/proc/self/fd/...` path and appropriate flags (e.g., `RTLD_NOW` or `RTLD_LAZY`).
        - **Check Result:** Check the return value of `dlopen`. `NULL` indicates an error. Log success or failure. _Note: The loaded library's initialization functions (`_init`, constructors) will run here._

**Hints and Considerations:**

- **ELF Headers:** You'll need `<elf.h>`. Make sure you are using the correct 64-bit structures (`Elf64_Ehdr`, `Elf64_Phdr`, `Elf64_Dyn`, `Elf64_Sym`) assuming a 64-bit target.
- **System Calls:** The example code uses raw system calls (e.g., `sys_memfd_create`, `sys_write`, `sys_close`). You might use these directly (requires `<sys/syscall.h>` and `syscall()`, or custom wrappers like in the example) or use the corresponding libc wrappers (`memfd_create`, `write`, `close`) if allowed/preferred. Define `_GNU_SOURCE` if using `memfd_create` from `libc`.
- **Pointer Types:** Be careful with pointer types and casting (`void*`, `uint8_t*`, `Elf64_Addr`, function pointers).
- **Dependencies:** Your loader code will rely on `search_lib_procmaps` from the previous assignment. Ensure it  works correctly.
 Once your object file is built, compile it it with your payload and test it out


## Creating Standalone Shellcode

While the shared object (`.so`) file contains the reflective loader and payload, it typically requires an external program or action (like a `dlopen` call) to load it and transfer execution to its entry point (`load_memfd`). For scenarios like exploits, you often need shellcode – a block of position-independent machine code that can be placed directly into memory and executed standalone.

We can convert our reflective shared object into shellcode by prepending a small "bootstrap" stub. This stub's sole purpose is to jump from the very beginning of the shellcode block to the actual loader entry point (`load_memfd`) within the main body of the payload.

**The Challenge: PC-Relative Addressing and Alignment**

* **Bootstrap Jump:** The bootstrap code needs to perform a jump (branch) relative to its own position (the current Program Counter, PC) to the `load_memfd` function.
* **Payload Alignment:** The main payload (the original `.so` content) contains ELF structures. Functions like `find_own_elf_base` often rely on finding the ELF header (`\x7FELF`) at a predictable alignment, usually a page boundary, relative to the code currently executing *after* the bootstrap jump. If the bootstrap code shifts the main payload data relative to the start of the *entire* shellcode block, these assumptions might break. The main payload needs to effectively start at a page-aligned offset from the beginning of the *combined* shellcode (bootstrap + payload) for the internal ELF parsing logic (especially `find_base`) to work reliably..

**Approach 1: Bootstrap Padding (Recommended)**

This is generally the simpler and more reliable method to ensure alignment. But it will bloat the size of the payload

1.  **Structure:** Create a bootstrap shellcode block that is exactly one memory page in size (e.g., 4096 bytes on many Linux systems).
    * Place a **PC-relative branch instruction** at the very beginning of this page.
    * Fill the *rest* of the page with **NOP (No Operation)** instructions.
2.  **Branch Target Calculation:** The branch instruction needs to jump to the `load_memfd` function, which now resides *after* the 4096-byte bootstrap.
    * Determine the offset of `load_memfd` relative to the start of the *original* `.so` file's content (`entry_point_offset`). You can find this using tools like `readelf` or `objdump` on the `rp.so` file.
    * The target address for the branch, relative to the start of the bootstrap page, will be `4096 + entry_point_offset`.
3.  **Assembly:**
    * Write the bootstrap in assembly language (e.g., using GNU `as`).
    * **Specify Architecture:** Ensure you use the correct branch and NOP instructions for your target architecture (e.g., `b <offset>` and `nop` for AArch64; 
    * **Padding:** Use assembler directives (like `.fill` or `.space`) or repeat the `nop` instruction thousands of times to precisely fill 4096 bytes after the initial branch instruction. Or just repeat the `nop` instruction as needed.  I reccomended making a python3 script to template the shellcode
    * **Alignment Awareness:** Be mindful that assemblers (like GNU `as`) might introduce padding to align instructions (e.g., to 16 bytes). Ensure your offset calculations account for the exact address of the branch instruction and its size, or use directives to control alignment if needed.

**Approach 2: Clobbering ELF Header (Advanced)**

Alternatively, you could place a *very short* bootstrap stub directly at the beginning of the `.so` file data, overwriting the ELF header.

* **Pros:** Avoids the large 4096-byte padding.
* **Cons:** This **breaks the ELF header**. The `find_own_elf_base` function (which searches for `\x7FELF`) will likely fail. You would need to modify the loader logic to determine its base address without relying on the ELF header, perhaps calculating it relative to the PC after the bootstrap jump. This adds some complexity. You will also likely want to fix the elf header after loading if the main payload requires a well formed elf header.
For that reason,  this assignment recommends using the padding approach. 4096 bytes isn't that big of a deal.

**Process for Combining (Using Padding Approach):**

1.  **Assemble Bootstrap:** Compile your assembly bootstrap code and copy out the text section from it.  (e.g., `bootstrap.bin`). Ensure that it is exactly 4096 bytes long.
```
 ls -la bin | grep bootstrap.bin
-rw-rw-r-- 1 user user  4096 Apr  6 22:48 bootstrap.bin
```

2.  **Create the  Payload:**  Append the bootstrap to the reflective shared object. 
    ```bash
    cat bootstrap.bin rp.so > final_shellcode.bin
    ```
    (Where `rp.so` is the  reflective shared object 

The resulting `final_shellcode.bin` file contains your standalone payload. When executed, the initial branch instruction will transfer control to your `load_memfd` function, which should then proceed with the reflective loading process, now operating on correctly page-aligned payload data relative to the start of the bootstrap page.

** Hard Mode: Reflective Dynamic Loader: Go Load Yourself**

Before starting this, make sure you completed the previous sections.

This challenging assignment requires you to implement a rudimentary userspace dynamic loader *within* position-independent code. Your loader, when executed, will need to:

1.  Locate necessary dynamic linking functions (`dlopen`, `dlsym`) within the already loaded `libc.so.6` in the process's memory space *without* initially linking against libc.
2.  Map the ELF file's own segments (program headers) into a *new* memory region.
3.  Parse the dynamic section of this newly mapped ELF image.
4.  Load any required shared library dependencies (`DT_NEEDED`) using the bootstrapped `dlopen`.
5.  Perform symbol lookups and apply ELF relocations (RELA, REL, PLT/JMP_SLOT) within the new memory region using the bootstrapped `dlsym`.
6.  Execute any initialization functions ( `DT_INIT_ARRAY`).

Essentially, you are rebuilding parts of `ld.so`'s functionality *reflectively*.

**Background:**

In the previous section, we loaded a simple, self-contained piece of code using `memfd_create` and `dlopen`.  This works because under the hood it calls the dynamic loader, which handles resolving  library dependencies  (like `libc`) and fixing up relocation entries.
In this assignment, you will not rely on `dlopen` to load your program (but you will use it to load dependencies after finding it at runtime in libc).
In particular,  the code you compile *itself* becomes the loader. It starts in a minimal environment (PIC, no standard libraries initially) and must *bootstrap* the ability to perform dynamic linking tasks on a target ELF (in this case, its own ELF image mapped into a new location). This involves manually parsing ELF structures, finding `libc` in memory, locating key functions within it by parsing its symbols, and then using those functions to load dependencies and fix up memory addresses (relocations).


1.  **Implement  the functions found int `reflective_loader.c`:** Create a C source file that will contain your reflective loader implementation.
2.  **Implement/Adapt the Loader Logic:**
    * Your main entry point should be `void *reflective_loader()`
        * Find the base address of `libc.so.6` in memory (e.g., using `search_lib_procmaps` from `procfsutil.h`).
        * Parse the ELF structure of the loaded `libc.so.6` to find the addresses of the `dlopen` and `dlsym` functions (e.g., using `find_symbol_symtab`). Store these as function pointers in a context structure (`struct dyn_loader_ctx`). Optionally find `printf` for debugging.
    * **Self-Mapping:** Implement or adapt logic to:
        * Find the base address of the *currently executing* loader code itself (e.g., `find_base`).
        * Map the program segments (PT_LOAD) of the loader ELF into a *new*, writable and executable memory region (e.g., `mmap_elf_segments`). This creates the target memory space where linking will occur.
    * **ELF Parsing:** Implement or adapt `process_elf_ctx` to parse the ELF headers and *dynamic section* of the *newly mapped* memory region, populating a structure (`struct elf_ctx`) with essential pointers (symbol table, string table, relocation tables, init arrays, needed libraries, etc.). Pay attention to calculating the `load_bias `(  `mapped_addr` - `lowest_addr`).
    * **Load Dependencies:** Implement or adapt `process_needed_libs`. Use the virtul address of  `dlopen` pointer to load all libraries listed in the `DT_NEEDED` entries found in the dynamic section. Keep track of the returned handles.
    * **Perform Relocations:** Implement or adapt `process_relocations`. This is the most complex part. Iterate through the relocation tables (`DT_RELA`, `DT_REL`, `DT_JMPREL`). For each entry:
        * Determine the relocation type (e.g., `R_AARCH64_RELATIVE`, `R_AARCH64_GLOB_DAT`, `R_AARCH64_JUMP_SLOT`). **Note:** The provided code is specific to AArch64 relocation types.
        * Identify the symbol involved (if any).
        * If the symbol is undefined (`SHN_UNDEF`), use the bootstrapped `dlsym` pointer and the handles from `process_needed_libs` to find the symbol's absolute address in a loaded dependency.
        * Calculate the correct value based on the relocation type, symbol address (`S`), load base address (`B`), and addend (`A`).
        * Write the calculated value to the target memory location specified by the relocation entry's offset.
        * note that if `dlsym` fails, this could mean that the symbol is `local`. I.e. it is  referencing a symbol contained in the library itself!
    * **Call Initializers:** Implement or adapt `process_init`. After relocations are complete, call the function specified by `DT_INIT` (if present) and iterate through the function pointers in the `DT_INIT_ARRAY` (if present), calling each one.
    * **Return:** The `reflective_loader` function should return the base address of the *newly mapped and processed* memory region, or `NULL` on failure.
3.  **Address `-nostdlib` Constraints:** The initial `reflective_loader.so` is compiled with `-nostdlib`. The provided reference code uses functions like `memcpy`, `memset`, and `strcmp`. These are part of `libc` and **will not be available** when your `reflective_loader` first starts executing. You must:
    * **Implement minimal versions** of `memcpy`, `memset`, `strcmp`, and any other required basic functions yourself within `reflective.c` (or in a separate file compiled and linked in). These need to function correctly without relying on any external libraries.
4.  **Implement/Obtain Helpers:** Ensure you have implementations for `search_lib_procmaps` (`procfsutil.h`) and the syscall wrappers (`syscall.h`). 
5.  **Compile:** Compile `tester.c` from Part 1 (`gcc tester.c -o tester`). Run `make` using your modified `Makefile` to build `bin/hard_loader.so`.
6.  **Test:** Run your reflective loader using the simple tester from Part 1:
    ```bash
    ./tester bin/hard_loader.so
    ```
    * Observe the debug output (`DEBUG_WRITE_LOG` or bootstrapped `printf`). Verify that the loader finds libc, finds `dlopen`/`dlsym`, maps segments, loads dependencies, processes relocations, and calls initializers without crashing.
    * The final return value printed by `tester` should be the non-NULL base address of the newly mapped region.

 To create a shellcode, leverage the same strategy employed for creating a memfd shellcode.



To sweeten the deal, if you can create a reflective shared object loader, that works with a stripped symbol table, (i.e. sstrip) I will bump you a full letter grade :-)
