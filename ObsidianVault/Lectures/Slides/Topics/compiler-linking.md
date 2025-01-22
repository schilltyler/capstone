

---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Converting Text Programs into Executables

:::

- What is a C/C++ compiler toolchain responsible for? Converting text (code) into an application that a CPU can run!
- This is accomplished in two main steps.
- **Compiling**
- **Linking**

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Compiling: Seriously Oversimplified

:::

- Converts source code (`.c/.cpp`) into object files (`.o`) containing machine code.
- The compiler performs various tasks:
    - Preprocessing (macros, includes)
    - Parsing and building an Abstract Syntax Tree
    - Generating machine code using its backend for AArch64
- The result of this stage: **Object files**

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Linking: Seriously Oversimplified

:::

- Once we have compiled object files, we need to **link** them together into an ELF executable.
- The linker resolves symbols (variables/functions) and stitches everything together:
    - References to undefined symbols are replaced with their correct addresses.
- We can share code in **libraries** to avoid duplication.
- Multiple ways to link against external code:

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Linking

:::

- **Static Linking**: External code gets included directly into your final executable.
    - Useful if you are unsure a needed library will be present on the target system.
    - Results in bigger binaries.
- **Dynamic Linking**: References to external libraries are stored symbolically in the binary.
    - At runtime, the loader (`ld.so`) loads these shared libraries.
    - Reduces binary size and promotes code reuse.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Shared Libraries (.so)

:::

- In Linux, shared libraries are typically `.so` (shared object) files.
- They contain exported, callable functions loaded at runtime.
- Examples:
    - `libc.so`: Core C library for syscalls, memory management, etc.
    - `libm.so`: Math library.
    - Other specialized libraries: `libssl.so`, `libcrypto.so`, etc.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Dynamic Linking

:::

- **Implicit Linking**
    - Your executable’s ELF headers declare which `.so` libraries it depends on.
    - At load time, if the loader can’t find them, the program can’t start.
- **Explicit Linking**
    - Programs can manually load libraries at runtime with something like `dlopen()`.
    - If loading fails, the program can decide how to handle that gracefully.
