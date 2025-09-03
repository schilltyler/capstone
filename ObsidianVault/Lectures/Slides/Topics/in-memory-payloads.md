

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### In Memory Payloads

:::
- code that executes without leaving forensic artifacts on disk  

![[in_mem_payload.png]]


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Techniques Covered Today

:::
- Anonymous Memory: RWX
- `tempfs`, `ramfs` and similar l 
- anonymous files/`memfd`



---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Basic shellcode 
:::

![[anon_shellcode.excalidraw.svg]]



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### c example

:::
- `man mmap`

snippet:
```c
 void *mem = mmap(NULL, sizeof(shellcode),
                     PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Check if memory allocation failed.
    if (mem == MAP_FAILED) {
        perror("mmap failed");
        return 1; // Return a non-zero code to indicate failure
    }

    // 2) Copy the shellcode bytes into the allocated memory region.
    memcpy(mem, shellcode, sizeof(shellcode));

    // 3) Execute the shellcode.
    printf("Executing shellcode...\n"); // This line will print

    // Cast the memory address to a function pointer taking no arguments and returning void.
    void (*func)() = (void (*)())mem;

    // Call the function pointer, executing the shellcode.
    func();
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### ramfs/tmpfs

:::
- logical file system that is **usually** not written to disk 
- file system exists in volatile memory 
- unlucky paging could write  data to disk
- examples include `/dev/shm`, and on some systems `/tmp`


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `man tmpfs`

:::
tmpfs(5)                                                                   File Formats Manual                                                                   tmpfs(5)

NAME
       tmpfs - a virtual memory filesystem

DESCRIPTION
       The  tmpfs facility allows the creation of filesystems whose contents reside in virtual memory.  Since the files on such filesystems typically reside in RAM, file
       access is extremely fast.
---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Finding a suitable location

:::
```
mount | grep tmpfs
udev on /dev type devtmpfs (rw,nosuid,relatime,size=3950656k,nr_inodes=246916,mode=755)
tmpfs on /run type tmpfs (rw,nosuid,nodev,noexec,relatime,size=824560k,mode=755)
tmpfs on /dev/shm type tmpfs (rw,nosuid,nodev)
tmpfs on /run/lock type tmpfs (rw,nosuid,nodev,noexec,relatime,size=5120k)
tmpfs on /run/user/1000 type tmpfs (rw,nosuid,nodev,relatime,size=824544k,nr_inodes=206136,mode=700,uid=1000,gid=1000)
```

```
mount | grep ramfs
ramfs on /run/credentials/systemd-sysusers.service type ramfs (ro,nosuid,nodev,noexec,relatime,mode=700)
ramfs on /run/credentials/systemd-sysctl.service type ramfs (ro,nosuid,nodev,noexec,relatime,mode=700)
ramfs on /run/credentials/systemd-tmpfiles-setup-dev.service type ramfs (ro,nosuid,nodev,noexec,relatime,mode=700)
ramfs on /run/credentials/systemd-tmpfiles-setup.service type ramfs (ro,nosuid,nodev,noexec,relatime,mode=700)
```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### In memory fs, that is rwx 

:::
```
mount | grep -v "noexec" | grep tmpfs 
udev on /dev type devtmpfs (rw,nosuid,relatime,size=3950656k,nr_inodes=246916,mode=755)
tmpfs on /dev/shm type tmpfs (rw,nosuid,nodev)
tmpfs on /run/user/1000 type tmpfs (rw,nosuid,nodev,relatime,size=824544k,nr_inodes=206136,mode=700,uid=1000,gid=1000)
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Using it like any other file system

:::
Copy file to system and run it
```
┌[user@spr-2025] [/dev/pts/0] [1]
└[~]> cp a.out /run/user/1000 
┌[user@spr-2025] [/dev/pts/0] 
└[~]> /run/user/1000/a.out 
Executing shellcode...
hello
```

```
└[~]> cp a.out /dev/shm
┌[user@spr-2025] [/dev/pts/0] 
└[~]> /dev/shm/a.out 
Executing shellcode...
hello
[1]    70827 illegal hardware instruction  /dev/shm/a.out
```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example: write file and execute

:::
```c
/// file has been written to out_path
   const char out_path[] = "/dev/shm/a.out";
  // Set executable permissions
  if (chmod(out_path, 0755)) {
    perror("chmod");
    return -1;
  }

  // Execute the file
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return -1;
  } else if (pid == 0) { // Child process
    execl(out_path, out_path, NULL);
    perror("execl");
    exit(EXIT_FAILURE);
  } else { // Parent process
    waitpid(pid, NULL, 0);
  }

  return 0;
}

```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Word of caution

:::
- while tmpfs and ramfs are usually not paged to disk it can happen
- users with same UID/root can still see these files even if they aren't written to disk 
- ie 

```
 ls  -lah /dev/shm
total 48K
drwxrwxrwt  2 root root   60 Mar 26 15:58 .
drwxr-xr-x 16 root root 4.1K Feb 19 17:07 ..
-rwxr-xr-x  1 user user  69K Mar 26 15:58 a.out
```
- forensic artifacts still need to be removed. 
- bright side: cleaning up RAM is a lot easier than cleaning up disk 
- Not every Linux system will have a tmpfs that allows for executable files
- usually shm ( `/dev/shm` ) exists on linux systems
- Takeaway: `/dev/shm` is only marginally safer than writing to actual disk 



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example: Epoch0

:::
- wrote payload to /dev/shm
- executed payload from /dev/shm
- OPSEC mistake: left payload there after spawning process 


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Anonymous Files 

:::
-
- file-like objects that are invisible to the normal filesystem.
- They are created using  `memfd_create` and can be used similarly to regular files (read, write, mmap).
- They reside purely in memory (RAM) and vanish when closed or when the creating process terminates.
- anonymous files can be executed 


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Anonymous file In memory Payload

:::
1. **No Persistence on Disk**  
   - Contents never touch permanent storage unless explicitly written out.
2. **Security & Isolation**  
   - Limits exposure of sensitive data to users or processes that can browse the filesystem. is *private* to the processes 
1. **Executable Dynamic Content**  
   - Load ELF binaries or scripts dynamically and execute them without ever writing them to disk.


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `memfd` with c

:::
- **Prototype**:

```c
  int memfd_create(const char *name, unsigned int flags);
```

0 Creates an anonymous file and returns a file descriptor.

    name: A string identifier (visible in /proc/<pid>/fdinfo/ but not on the real filesystem).

    flags:

        MFD_CLOEXEC to close FD on exec

        MFD_ALLOW_SEALING to allow applying seals

        MFD_NOEXEC_SEAL or MFD_EXEC (on newer kernels) to control executability

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Executing a payload with `memfd`

:::
- **Create an Anonymous File**
    - Use `memfd_create()` or `syscall(SYS_memfd_create, "my_memfd", flags)`.
        
- **Write ELF Binary into the Anonymous File**
    - A valid ELF binary must be written in order for it to be executable via `fexecve()`.
        
- **Adjust Permissions** (if necessary)
    - `fchmod(fd, 0700)`  mark memory file as  executable.
        
- **Execute the Anonymous File**
    - Use `fexecve(fd, argv, envp)` to run the ELF 



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Hunting for `memfd`

:::

Although `memfd` files do not reside on disk, you can still observe their presence in a process by looking at **/proc** or using tools like **lsof**:

1. **Check /proc/\<pid\>/fd**  
   - Each process has a directory in `/proc` named after its PID. Inside `/proc/<pid>/fd`, you’ll see symbolic links for each open file descriptor.
   - If a process has a memfd open, it often appears with a link name like `memfd:my_memfd (deleted)` or a similar marker.
   - 
   
```bash
   ls -l /proc/<pid>/fd
   lsof -p <pid> | grep memfd
   cat /proc/<pid>/maps | grep memfd
```




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Why not to use `memfd`

:::
- not all Linux systems will support it, especially older ones 
- executing elf files from `memfd` is a common hunting rule 
- while effective, is easily detected



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### In Memory Payloads: Elf Loaders

:::
- TLDR: Given a region of memory that is RWX,  do everything that `ld.so` would do given your elf file. Then, execute the entry point
- This is harder to detect when done properly than memfd
- There are many ways to load an elf file, and it often involves strategies similar to running shellcode 


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Baby's first soloader

:::
- Everyone at some point should write an shared object loader
- It doesn't need to handle everything that `ld.so` does, but it needs to do the bare minimum
- By bare minimum, think hello world program that uses plt.got





---
<!-- slide template="[[Base Slide]]" -->

::: title

#### soloader steps

:::
- Memory map segment s
- Perform relocations 
- Call init_array function
- profit 



---
<!-- slide template="[[Split Vertical]]" -->
::: title

####  soloader in depth

::: 
::: left 
- find the base address of `libc.so.6`
- memory map segments (we know how to do this)
	- we can use direct syscalls for this: mmap, mprotect 
- find all functions we might need from there (`dlopen, dlinfo, dlsym...etc`)
::: 
::: right

![[find_dlopen.png]]

:::


---
<!-- slide template="[[Split Vertical]]" -->
::: title

#### Why not just Shellcode

::: 
::: left 
- its hard 
- It is difficult to make portable code 
- You lose a lot of functionality that is ergonomic for development 
- makes integrating other code bases really annoying 
::: 
::: right

![[shellcode_hard.png|300]]
 
:::



---
<!-- slide template="[[Base Slide]]" -->

::: title

####  Find Libc base address
:::
- use procfs
	- `/proc/self/maps`: search memory map for `libc`
	- `/proc/self/auxv` use auxiliary vector to find the link map, addres of dynamic loader, or the executable 



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Relocations

:::
- `readelf -r`

```bash
 readelf -rW hello

Relocation section '.rela.dyn' at offset 0x480 contains 8 entries:
    Offset             Info             Type               Symbol's Value  Symbol's Name + Addend
000000000001fdc8  0000000000000403 R_AARCH64_RELATIVE                        750
000000000001fdd0  0000000000000403 R_AARCH64_RELATIVE                        700
000000000001ffd8  0000000000000403 R_AARCH64_RELATIVE                        754
0000000000020030  0000000000000403 R_AARCH64_RELATIVE                        20030
000000000001ffc0  0000000400000401 R_AARCH64_GLOB_DAT     0000000000000000 _ITM_deregisterTMCloneTable + 0
000000000001ffc8  0000000500000401 R_AARCH64_GLOB_DAT     0000000000000000 __cxa_finalize@GLIBC_2.17 + 0
000000000001ffd0  0000000600000401 R_AARCH64_GLOB_DAT     0000000000000000 __gmon_start__ + 0
000000000001ffe0  0000000800000401 R_AARCH64_GLOB_DAT     0000000000000000 _ITM_registerTMCloneTable + 0

Relocation section '.rela.plt' at offset 0x540 contains 5 entries:
    Offset             Info             Type               Symbol's Value  Symbol's Name + Addend
0000000000020000  0000000300000402 R_AARCH64_JUMP_SLOT    0000000000000000 __libc_start_main@GLIBC_2.34 + 0
0000000000020008  0000000500000402 R_AARCH64_JUMP_SLOT    0000000000000000 __cxa_finalize@GLIBC_2.17 + 0
0000000000020010  0000000600000402 R_AARCH64_JUMP_SLOT    0000000000000000 __gmon_start__ + 0
0000000000020018  0000000700000402 R_AARCH64_JUMP_SLOT    0000000000000000 abort@GLIBC_2.17 + 0
0000000000020020  0000000900000402 R_AARCH64_JUMP_SLOT    0000000000000000 printf@GLIBC_2.17 + 0
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Runtime Addresses?

:::
* **Static Linking:** All code/data bundled. Addresses fixed at link time. Simple, but large & inflexible.
* **Dynamic Linking:** Use shared libraries (`.so`). Loaded at runtime. Saves space, easier updates.
* **Position-Independent Executables (PIE):** Executable loaded at random address (ASLR).

**Challenge:**
How does code find its functions/data if their final memory address isn't known until load time?


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Dynamic Relocations

:::

* **What:** Patches applied to code/data *at load time*.
* **Who:** Performed by the **Dynamic Linker** (e.g., `/lib64/ld-linux-x86-64.so.2`).
* **Why:** To resolve references (addresses) based on the actual load locations of the executable and its shared libraries.
* **How:** Using **Relocation Entries** embedded in the ELF file by the static linker (`ld`).


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Relocatable Binaries

:::

 **Shared Libraries (`.so`):**
    * Must run correctly at *any* base address they are loaded at.
    * Cannot contain hardcoded absolute addresses pointing within themselves.
* **Position-Independent Code (PIC):**
    * Code generation style that avoids absolute addresses.
    * Uses relative addressing or indirect access via tables (GOT/PLT).
* **Position-Independent Executables (PIE):**
    * Executables compiled using PIC. Enables ASLR.

**Dynamic relocations are essential for all of these.**



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Sections Relevant to Relocations

:::

* **Global Offset Table (`.got` / `.got.plt`):**
    * A table located in the data section.
    * Stores *absolute addresses* of global variables or functions needed by the code.
    * PIC code reads the required address from the GOT instead of using a fixed address.
    * Dynamic relocations fill in the correct addresses here.

* **Procedure Linkage Table (`.plt`):**
    * A table located in the code section with small executable "stubs".
    * Used by PIC to call functions in other modules (or even within PIE).
    * Works with the GOT to enable **lazy binding** (resolve function address only on first call).


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Performing Relocations

:::

* The  linker (`ld`) identifies code/data needing runtime patching.
* It stores instructions for the dynamic linker in **relocation sections**:
    * `.rela.dyn`: Usually for data pointers, GOT entries.
    * `.rela.plt`: Usually for function calls via the PLT/GOT.
    * (`.rel.dyn` / `.rel.plt` on older/32-bit systems).

* **Each entry (`Elf64_Rela`) specifies:**
    * `r_offset`: The location (virtual address offset) within the file's mapping to patch.
    * `r_info`: Contains the **Symbol** index and **Relocation Type**.
    * `r_addend`: An explicit value to add during the relocation calculation.


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Relocation Types

:::

*(Architecture Specific! Examples for AArch64 / x86-64)*

* `R_..._RELATIVE`:
    * Adjusts internal pointers based on the module's actual load address.
    * Calculation: `BaseAddress + Addend`.
* `R_..._GLOB_DAT`:
    * Finds the absolute address of a data symbol, often to store in the GOT.
    * Calculation: `SymbolAddress + Addend`.
* `R_..._JUMP_SLOT` (or similar):
    * Finds the absolute address of a function symbol, typically for PLT entries.
    * Calculation: `SymbolAddress (+ Addend)`.
    * Often handled *lazily*. Our loader cannot handle this lazily 

*(Replace `...` with `AARCH64` or `X86_64` etc.)*



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Reminder

:::

1.  Kernel loads executable & maps `ld.so`. `ld.so` takes control. <-- we did this
2.  `ld.so` finds and `mmap`s required shared libraries (from `DT_NEEDED` tags).
3.  **For each object** (executable & libraries):
    * Parses ELF structures.
    * Iterates through relocation entries (`.rela.dyn`, `.rela.plt`).
    * For each entry:
        * Find symbol's address (`S`).
        * Read addend (`A`).
        * Calculate `Value` based on relocation type (e.g., `S+A`, `Base+A`).
        * Write `Value` to the memory location `r_offset`. <-- *(This is the relocation!)*
4.  Calls initializers (`.init_array`).
5.  Transfers control to the executable's entry point (`_start`).



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Cheat Sheet

:::

```

* `readelf -r <file>`: Show relocation entries.
* `readelf -d <file>`: Show dynamic section (`DT_NEEDED`, `DT_REL`, etc.).
* `readelf -s <file>`: Show symbol table.
* `readelf -a <file>`: Show everything.
* `objdump -R <file>`: Show dynamic relocation entries.
* `objdump -d <file>`: Disassemble code (see `*.plt` calls, GOT references like `[rip + ...]`).
* `ldd <executable>`: List dynamic library dependencies.
* `gdb`: Examine memory (`x/gx`), step through `ld.so` or PLT stubs.

```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Re locations: fixup 

:::
What do these do?
- `R_AARCH64_RELATIVE`
- `R_AARCH64_GLOB_DAT`
- `R_AARCH64_JUMP_SLOT`
- there isn't a man page that describes what each of these do
- your best bet is to reference other loaders 
- For example, you can search source code for `ld.so`
- https://github.com/search?q=repo%3Abminor%2Fglibc+R_AARCH64_GLOB_DAT&type=code
- hint for homework:

```
void *sym_addr = dlsym(RTLD_DEFAULT, sym_name);
```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `.init_array`

:::

*  A special section (`.init_array`) in the ELF file containing an array of **function pointers**.
* Used to execute initialization code *before* `main()` runs.
    * C++ static constructors (objects with static storage duration).
    * Library setup routines (e.g., registering components, initializing subsystems).
    * General pre-main setup.
    * payloads that execute on `dlopen` :-)



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `sample.so`

:::
`__attribute__((constructor))`
```c
#include <stdio.h>

// This function will be called when the shared object is loaded
void hello_world() {
    printf("Hello World from .init_array!\n");
}

// Constructor attribute method (alternative approach)
__attribute__((constructor))
void hello_world_constructor() {
    printf("Hello World from constructor attribute!\n");
}

// Dummy function to make our library useful
void dummy_function() {
    printf("This is a dummy function\n");
}

// The .init_array approach
// We'll create a function pointer array and place it in .init_array section
void (*init_array[])(void) __attribute__((section(".init_array"))) = { hello_world };
```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `.init_array` continued

:::

1.  **Link Time:** The linker collects pointers to functions marked as initializers (e.g., via compiler attributes) and stores them in the `.init_array` section.
2.  **Load Time Execution Flow:**
    * `ld.so` loads executable & libraries, performs relocations.
    * `ld.so` transfers control to the executable's entry point (usually `_start` in the C Runtime - CRT).
    * **CRT code (`_start` etc.):**
        * Sets up the environment (stack, argc/argv).
        * **Iterates through the function pointers in `.init_array` (from lowest to highest address, or respecting priorities) and calls each function.**
        * Calls `main()`.
    * After `main` returns (or `exit()` is called):
        * CRT calls cleanup functions from `.fini_array` (destructors, reverse order of `.init_array`).


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### .init_array

:::
- ctors/dtors: code that is run on load/unload 

```
readelf -d sample.so

Dynamic section at offset 0xfe00 contains 24 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x000000000000000c (INIT)               0x550
 0x000000000000000d (FINI)               0x704
 0x0000000000000019 (INIT_ARRAY)         0x1fde0
 0x000000000000001b (INIT_ARRAYSZ)       24 (bytes)
 0x000000000000001a (FINI_ARRAY)         0x1fdf8
 0x000000000000001c (FINI_ARRAYSZ)       8 (bytes)
 0x000000006ffffef5 (GNU_HASH)           0x1f0
 0x0000000000000005 (STRTAB)             0x348
 0x0000000000000006 (SYMTAB)             0x228
 0x000000000000000a (STRSZ)              173 (bytes)
 0x000000000000000b (SYMENT)             24 (bytes)
 0x0000000000000003 (PLTGOT)             0x1ffe8
 0x0000000000000002 (PLTRELSZ)           72 (bytes)
 0x0000000000000014 (PLTREL)             RELA
 0x0000000000000017 (JMPREL)             0x508
 0x0000000000000007 (RELA)               0x430
 0x0000000000000008 (RELASZ)             216 (bytes)
 0x0000000000000009 (RELAENT)            24 (bytes)
 0x000000006ffffffe (VERNEED)            0x410
 0x000000006fffffff (VERNEEDNUM)         1
 0x000000006ffffff0 (VERSYM)             0x3f6
 0x000000006ffffff9 (RELACOUNT)          3
 0x0000000000000000 (NULL)               0x0
```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Finding `init_array`

:::

```bash
objdump -s -j .init_array sample.so 

sample.so:     file format elf64-littleaarch64

Contents of section .init_array:
 1fde0 a0060000 00000000 00000000 00000000  ................
 1fdf0 00000000 00000000                    ........ 
```

---
<!-- slide template="[[Split Vertical]]" -->
::: title

####  "reflective so injection" 

::: 
::: left 
- Shared object that is able to load itself
- elf is static and has no dependencies that it  cannot resolve on its own
- i.e. it assumes `libc/ld.so`is in the process but doesn't require them  
- simply copy the shared object as it exists on disk into memory, and execute the reflective loader
- given a simple bootstrap shellcode, this can turn  a shared object into a payload that is functionally similar to position independent shellcode 
::: 
::: right
![[pic_loader.png]]
:::



---
<!-- slide template="[[Split Vertical]]" -->
::: title

####  Reflective Loader 

::: 
::: left 
- exported function that does not use libc, plt. got,...etc
- Is responsible for loading itself 
- memory mapping segments
- performing relocations 
- tls, bss ...etc
- Execute init_array 
::: 
::: right
![[load_yourself_elf.png]]

:::