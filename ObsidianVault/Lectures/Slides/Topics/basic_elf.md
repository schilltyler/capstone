
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### ELF File Format: Basic Definitions 
:::

- **ELF** (Executable and Linkable Format) is used by Linux (and many other Unix-like OSes)
- It describes how code and data are laid out, plus metadata for loading
- Tools like `readelf`, `objdump`, `file` help examine ELF files

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### ELF File Format on Linux/aarch64
:::

- Used for both userland and kernel modules
  - Userland: typical `.out`, `.so`, or no extension at all  
  - Kernel modules: `.ko` files  
- Architecture is specified inside the ELF headers (e.g., 0xB7 for AArch64, etc.)
- Data is grouped into segments and sections. Each section has a header with layout info

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Shared Libraries
:::

- On Linux, libraries are typically `.so` (“shared object”) files
- They export functions that other ELF executables can dynamically link against
- This is how large programs remain modular
- We’ll see more on linking in a future lecture

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### ELF Sections
:::

- Typical sections you’ll see:
  - `.text`: executable instructions
  - `.rodata`: read-only data
  - `.data`: mutable global data
  - `.bss`: uninitialized data
  - `.got/.plt`: used for dynamic linking
  - `.symtab`/`.strtab`: symbol tables  
- The **Program Header Table** also describes how segments map into memory
