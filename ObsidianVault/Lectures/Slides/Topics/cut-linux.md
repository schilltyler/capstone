
---
## Demo 0: Hello world on ARM64

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### What Exactly Happened?

:::

- The compiler transforms `.c` source into `.o` object files.
- The linker then combines `.o` files plus libraries into an ELF binary.
- glibc or musl can be statically or dynamically linked.
- The kernel loads the ELF, sets up the stack, and runs `_start`.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Converting Source to Binaries

:::

1. **Compilation**
    - Preprocessing (macros, includes).
    - AST generation and machine code emission.
    - Produces `.o` files.
2. **Linking**
    - Resolves symbols (function calls, global variables).
    - Stitches together `.o` files + libraries into a final ELF binary.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Linking: Static vs. Dynamic

:::

- **Static Linking**: Library code is included in the final binary.
    - Larger executables, but no external dependencies needed at runtime.
- **Dynamic Linking**: The linker sets up references to `.so` files loaded at runtime.
    - Smaller binaries, code sharing, but requires the matching shared library installed.

---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Shared Object (so) Files

:::

- ELF file with library code (exported functions/symbols)
- Minimizes code duplication in memory.
- Tools like `ldd` show which `.so` dependencies your binary has.
- Common dependencies: `libc.so`, `libpthread.so`, `librt.so`, etc.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Interacting With Linux

:::

- **System calls**: The ultimate gateway to kernel land.
- glibc function calls: Documented, stable, typically prefer these.
- **Direct Syscalls**: Less common, used in specialized or minimal environments.

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example: Simple I/O

:::

```c
#include <stdio.h>

int main(){
    FILE *fp = fopen("test.txt", "w");
    if(fp){
        fprintf(fp, "Hello from ARM64 Linux!\n");
        fclose(fp);
    }
    return 0;
}
```

- Under the hood, calls `open()`, `write()`, and `close()`.
- `strace`: syscall trace
---

## Demo:

Compiling the Example & Linking Against glibc
