---
slideNumber: true
theme: night
---

---
<!-- slide bg="[[Green Logo.svg]]" background-size="auto" -->


---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### CS-4973/6983
:::
::: left
![[Green Logo.svg | 400]] <!-- element style="object-fit: cover" -->

:::
::: right
<!-- element style="font-size: 32px;align:center" -->
Introduction to Malware, Threat Hunting & Offensive Capabilities Development
:::




---
::: title

#### CS-4973

::: ::: left ![[Green Logo.svg | 400]] ::: ::: right

Introduction to Malware, Threat Hunting & Offensive Capabilities Development  
(_Linux on AArch64 Edition_) :::

---

## Lecture 0x04:

C, C++ and Static Analysis (for Linux / AArch64)

---

::: title

#### Agenda for This Lecture

:::

::: left

- C/C++ on Linux
- Ghidra
- GNU/Linux syscalls
- GCC, Clang, Zig
- ARMv8-A Assembly
- Calling Conventions (AArch64) :::

::: right ![[0x05_slide_3_image.jpg|300]] :::

---

::: title

#### Example of a Missing .so Crash

:::

::: left

- Run an ELF binary that depends on a `.so` that isn't installed, you get:
    
    ```
    error while loading shared libraries: libX.so: cannot open shared object file
    ```
    
- This occurs when the dynamic loader fails to find a required library.
- For some homework crackmes, you may need to compile or link your own libraries! :::

::: right ![[0x05_slide_10_image.png|300]] :::

---

::: title

:::

::: left

_(Blank / filler slide for images)_ :::

::: right ![[0x05_slide_11_image.png|300]] :::

---

::: title

#### Compiling and Linking

:::

- Compiler frontends like `gcc/g++/clang/zig` can do both compilation and linking in one command.
- You can pass linker options like `-lssl`, `-lcrypto`, etc.
- By default on most Linux systems, linking against `libc.so` is automatic.

---

::: title

#### Interacting with the Linux OS

:::

- **GNU C Library (glibc)**: Provides documented, callable functions that wrap Linux syscalls: `printf`, `malloc`, etc.
- **Syscalls**: The actual system calls, typically invoked via a CPU instruction (`svc 0` on AArch64) and enumerated in `<sys/syscall.h>`.
    - e.g., `read()`, `write()`, `mmap()`, `execve()`, etc.
- **Kernel-internal functions**: Strictly in kernel mode, not accessible from userland.

---

::: title

#### libc.so

:::

- Implements the standard C library routines on Linux.
- It is the userland interface to the kernel for most core functionality.
- Most processes will link to `libc.so` by default.

---

::: title

#### Using Documented vs. Undocumented APIs

:::

- Typically, we use well-documented library calls like `fopen()`, `printf()`, etc.
- These library calls eventually make kernel syscalls under the hood.
- In advanced RE or exploit dev, we sometimes directly invoke syscalls.
- Linux changes syscalls from time to time, but glibc helps maintain backward compatibility.

---

::: title

#### Common Linux Data Types

:::

- `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`: fixed-width integer types from `<stdint.h>`.
- `size_t`: an unsigned integer type typically the size of a pointer (64 bits on AArch64).
- `char *`: pointer to (ASCII/UTF-8) strings, null-terminated.
- `wchar_t`: wide character type (on Linux often 4 bytes, but rarely used outside certain locales).
- `void *`: pointer to any type.
- **Consult** [cppreference.com](https://en.cppreference.com/) or man pages as needed.

---

::: title

#### Example: Printing a message

:::

```c
#include <stdio.h>

int main(){
    printf("Hello there!\n");
    return 0;
}
```

---

## Demo:

Compiling the example and linking

---

::: title

#### Reading the Docs

:::

::: left

- “Debugging your code for 8 hours can save you 5 minutes of reading docs.”
    - _Yes, we’ve all done it. Let’s be better._
- Example: `man 3 printf` or online references (e.g. [cppreference.com](https://en.cppreference.com/)). :::

::: right ![[0x05_slide_21_image.png|300]] ![[0x05_slide_21_image.png|300]] :::

---

::: title

#### Strings in Linux

:::

::: left

- In C, strings are sequences of `char` in memory, ending in a null byte (`'\0'`).
- Typically UTF-8, but not always.
- This means you can’t do random access if the string is multi-byte encoded.
- For non-ASCII or multi-language text, you might need wide characters (`wchar_t`). :::

::: right ![[0x05_slide_22_image.png|500]] :::

---

::: title

#### Strings and Character Encodings

:::

- Linux (and modern applications) largely favor UTF-8.
- But you can still handle wide strings via `<wchar.h>` if needed.
- The standard library provides `strlen, strcmp, strcpy, etc.` for normal strings and `wcslen, wcscpy, etc.` for wide strings.

---

::: title

#### Coding Conventions for this Class

:::

- We'll keep function names in `snake_case` or `PascalCase`—just be consistent.
- Use the C++ standard library (`<vector>`, `<string>`, etc.) as needed.
- Keep functions short if possible (< 20 lines).

---

::: title

#### C++ Usage in this Class

:::

- This isn’t a C++-only course, but we’ll use some features like:
    - `nullptr`
    - `auto`
    - `new/delete` or smart pointers
    - Classes, templates, etc.
- For the final project, you can use C or C++ on Linux/AArch64.

---

::: title

#### General Advice for Strings

:::

- You can just stick to UTF-8 with `printf`/`scanf`.
- If you need wide characters, you'll use `wprintf`/`fwscanf`, etc.
- For most reverse-engineering tasks on Linux, ASCII/UTF-8 is easiest.
- If you see weird encodings in malware, you might need advanced approaches.

---

::: title

#### AArch64 Assembly Crash Course

:::

::: left

- Assembly language is architecture-specific.
- ARMv8-A (AArch64) has 31 general-purpose registers: `x0`–`x30`, plus `sp` (stack pointer) and `pc` (program counter).
- Some registers have special usage:
    - `x0` often used for function return values.
    - `x0`–`x7` typically used for arguments in many Linux ABIs.
- We’ll revisit details as we reverse-engineer. :::

::: right ![[0x05_slide_30_image.png|300]] :::

---

::: title

#### Common Registers (AArch64)

:::

- **x0–x7**: used to pass function arguments, return values.
- **x8**: indirect syscall param or special usage.
- **x9–x15**: temporary registers.
- **x16–x17**: intra-procedure call scratch regs.
- **x19–x28**: callee-saved registers.
- **x29**: frame pointer (often used with `fp` alias).
- **x30**: link register (return address).
- **sp**: stack pointer.
- **pc**: program counter (auto-updated).

---

::: title

#### Refresher: Types

:::

::: left

- Byte: 8 bits
- Halfword: 16 bits
- Word: 32 bits
- Doubleword: 64 bits
- ARM is also **little-endian** in memory.
- In assembly, you’ll see mnemonics for loading/storing different widths (e.g. `ldrb`, `ldrh`, `ldr`, `ldur`). :::

::: right ![[0x05_slide_32_image.png|300]] :::

---

## Declaration Mapping

---

## Common Instructions

---

::: title

#### Calling Conventions

:::

- A _calling convention_ defines how functions get parameters and how they return results.
- On Linux/AArch64, arguments are typically passed in x0, x1, x2, x3, etc.
- Return values often come back in x0.
- Custom calling conventions exist (obfuscation in malware, etc.), so be aware.

---

::: title

#### Linux AArch64 Calling Convention: Basics

:::

- Up to 8 integer or pointer parameters go into **x0–x7**.
- If more arguments exist, they go on the stack.
- The **return value** is in **x0**.
- **x9–x15, x0–x7** are generally caller-saved (volatile).
- **x19–x28** are callee-saved (preserved across function calls).
- Reference: [ARM Procedure Call Standard](https://developer.arm.com/documentation/ihi0055/latest).

---

::: title

#### How We’ll Learn AArch64 in This Class

:::

- The best way: keep reading assembly in Ghidra or other tools.
- You won’t need to _write_ large chunks of assembly, but you _will_ need to read it.
- Decompilers can get confused or misinterpret code. The raw assembly is the ultimate ground truth.

---

::: title

#### Looking at Assembly

:::

::: left

- We’ll look at small examples of Linux/AArch64 assembly.
- Ghidra can show ARM assembly in its disassembly view and do decompilation.
- Examples:
    - A minimal “hello world”
    - A function that manipulates memory
    - Crackme code snippets :::

::: right ![[0x05_slide_38_image.png|300]] :::

---

::: title

#### Ghidra

:::

::: left

- Free & open-source reverse-engineering framework from the NSA.
- Supports many CPU architectures (including ARMv8-A).
- Decompiler is quite good, comparable to paid alternatives like IDA Pro. :::

::: right ![[0x05_slide_39_image.png|300]] :::

---

## Using Ghidra to Look at Our Code

---

::: title

#### Example: Minimal Assembly

:::

```asm
    .global _start       // Our entry point for a bare-metal example
    .section .text

_start:
    // sys_write(1, msg, len)
    mov   x0, #1        // file descriptor: stdout
    adrp  x1, msg       // load address of msg into x1 (PC-relative)
    add   x1, x1, :lo12:msg
    mov   x2, #13       // length of "hello, world\n"
    mov   x8, #64       // syscall number for write on ARM (varies by platform!)
    svc   #0

    // sys_exit(0)
    mov   x0, #0
    mov   x8, #93       // syscall number for exit
    svc   #0

    .section .rodata
msg:
    .ascii "hello, world\n"
```

---

::: title

#### Stack: What is it Good For?

:::

- Local variables
- Temporaries and function call frames
- Passing arguments beyond x0–x7
- Return addresses (in some calling conventions—though ARM uses x30 for LR)

---

::: title

#### Stack Memory

:::

- A region of virtual address space that grows downward.
- Each function call allocates a new **stack frame**.
- Freed upon function return.
- Typically safer/faster than the heap, but limited size.
- You must know how much you need at compile time (for local arrays, etc.).

---

::: title

#### Stack Memory

:::

- The memory is _temporary_: once a function returns, that local memory is reclaimed.
- Potential pitfalls: stack overflows, referencing freed stack frames, etc.

---

::: title

#### Stack-Based Footguns

:::

::: left

- Buffer overflows
- Using pointers to stack memory after the function returns
- Returning a pointer to local data—**don’t do it!** :::

::: right ![[0x05_slide_45_image.png|300]] :::

---

::: title

#### Heap Memory

:::

::: left

- Dynamically allocated at runtime (`malloc`, `new`).
- Freed with `free`, `delete`.
- Slower than stack, but flexible size.
- Keep track of memory usage to avoid leaks. :::

::: right ![[0x05_slide_46_image.png|300]] :::

---

::: title

#### Heap Allocation

:::

- The _heap allocator_ (in libc) manages large chunks of memory.
- Not freed automatically: you must `free()` what you `malloc()`.
- Memory leaks happen when code forgets to free.

---

::: title

#### C/C++ in a Nutshell

:::

::: left

:::

::: right ![[0x05_slide_48_image.png|300]] :::

---

::: title

#### Footguns. So Many Footguns

:::

::: left

- **Double free**: calling `free(ptr)` twice on the same pointer.
- **Use-after-free**: referencing memory after it’s freed.
- Both lead to undefined (and often exploitable) behavior. :::

::: right ![[0x05_slide_50_image.png|300]] :::

---

::: title

#### Dead Squirrels: Uninitialized Memory

:::

::: left

- A variable declared but never set can contain anything.
- Reading it yields “garbage” data = chaos. :::

::: right ![[0x05_slide_51_image.png|300]] :::

---

::: title

#### Uninitialized Memory

:::

- e.g. `int x; // x is uninitialized`
- The contents can be random.
- Always initialize your variables.

---

::: title

#### Memory Corruption

:::

- Failing to free allocations => memory leaks.
- Freed memory that gets used again => use-after-free.
- Overwriting memory out of bounds => buffer overflow.

---

::: title

#### Returning Stack Variables

:::

- If you do `char *foo() { char arr[10]; return arr; }`, once you `return`, `arr` is out of scope.
- Accessing it afterwards leads to undefined behavior.

---

::: title

#### HW Is Up!

:::

::: left

- Due in two weeks, but start ASAP!
- You’ll encounter _CrackMes_—little puzzle binaries for practicing reverse engineering.
- The goal: Find the input that makes the binary print “Cracked!”
- This assignment helps you get comfortable with basic assembly, finding `main`, etc.  
    :::

::: right ![[0x05_slide_55_image.png|300]] :::

---

::: title

#### Get Started Soon!

:::

::: left

- If you’re new to reversing, it’s best to start early.
- Unpredictable gotchas can eat up time.
- The CrackMes here aren’t “mean,” but they aren’t trivially straightforward either. :::

::: right ![[0x05_slide_56_image.png|200]] :::

---

::: title

#### CrackMes 2021: A Savage Hypocrisy

:::

::: left

- Let’s look at some older CrackMes used in a previous edition of this class.
- We’ll focus on the logic for the first few.
- They’re great practice for stepping through code in Ghidra or other tools. :::

::: right ![[0x05_slide_57_image.png|300]] :::

---

::: title

#### Analysis Playbook

:::

![[static_analysis_playbook.png|500]]

---

::: title

#### Finding the Actual `main`

:::

- Sometimes there’s a bunch of startup code before `main` (e.g., `_start`, glibc init).
- In simpler CrackMes, just find the function that calls `exit()` or returns an `int`.
- TLS callbacks or other tricks can hide the “true” entry. But in these assignments, it’s more straightforward.
- Cross-references (X-refs) to “Success!” or “Failure!” strings often reveal the main routine.

---

::: title

#### Finding main: CrackMes

:::

- Run the binary: see a failure message. Search for it in Ghidra.
- Or look for the success message.
- `main()` typically returns an int, calls `exit()`, or has references to standard I/O calls.
- Check imports with `objdump -T crackme` or in Ghidra’s “Imports” tab.

---

::: title

#### CrackMe 0

:::

::: left

- Difficulty: trivial (once you find `main`).
- In Ghidra, you’ll see it compare your input to a hardcoded string.
- Bingo, done. :::

::: right ![[0x05_slide_61_image.png|300]] :::

---

::: title

#### CrackMe 1

:::

::: left

- Difficulty: easy.
- Checks if input length is 26 and if it starts with `'C'`.
- Possibly modifies or checks characters in a loop. :::

::: right ![[0x05_slide_62_image.png|300]] :::

---

::: title

#### CrackMe 2 (Learn from My Mistakes!)

:::

::: left

- Difficulty: moderate.
- Possibly manipulates characters with a do-while loop.
- Watch for off-by-one or weird transformations. :::

::: right ![[0x05_slide_63_image.png|300]] :::

---

::: title

#### CrackMe 2 (Again, Learn from My Mistakes!)

:::

- A stack buffer holds some ASCII-range data.
- Loops over `argv[1]`, comparing each character to some offset in that buffer.
- If the check fails, it prints “Fail”; otherwise “Success.”
- Reverse-engineer the transformation logic to figure out the correct input.

---

::: title

#### CrackMe 2 (...Oops)

:::

::: left

- Moral of the story: Don’t wait until the night before to code (or to reverse-engineer).
- You’ll make silly mistakes or leave in debug prints.
- Keep commit messages professional if you’re sharing code—someone might see them! :::

::: right ![[0x05_slide_65_image.png|300]] :::

---

::: title

#### General Advice

:::

- Follow the “flow chart” approach to reversing:
    1. Identify imports or strings.
    2. X-ref them.
    3. Analyze the relevant functions.
    4. Confirm your findings by re-checking in a debugger or with test inputs.
- Tools: Ghidra, `objdump`, `readelf`, `gdb`, Python scripts for automation.
- Avoid rabbit holes—CrackMes can be solved systematically.

---

**End of Adapted Slides**