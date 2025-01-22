

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### GNU as

:::
- Low-level programming language for ARMv8-A architecture
- Part of GNU toolchain (`as`, `ld`, `gcc`)
- Platform-specific directives
- Macro support
- Integration with C toolchain



---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Basic Syntax

:::

- ARMv8 assembly  written in “GNU as” style:
    
    ```
    <mnemonic> <destination>, <source1>, <source2>
    ```
    
- Example:
    
    ```
    ADD x0, x1, x2   ; x0 = x1 + x2
    SUB x3, x3, #1   ; x3 = x3 - 1
    ```
    
- Suffixes like `S` can update condition flags:
    
    ```
    ADDS x0, x1, x2
    ```
    

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Toolchains

:::
1. **Assemble** (as): `aarch64-linux-gnu-as program.s -o program.o`
2. **Link** (ld/gcc):  `aarch64-linux-gnu-ld program.o -o program`
3. **Execute**:
	./program

	- Note `aarch64-linux-gnu-gcc -nostdlib program.o -o program` can be used to combine  assembling and linking into one step


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Getting started

:::
- `gcc/as/ld` expects a symbol named `_start` for the entrypoint but this can be overridden. 
- data is organized into sections (`.text, .data` ...etc)
- comments: `//comment /* my multi */`
	- note there are other ways, but  stick to this for clarity
- Read https://ftp.gnu.org/old-gnu/Manuals/gas/html_chapter/as_7.html

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Sections in gnu as 

:::

| Section | Purpose               | Example               |
| ------- | --------------------- | --------------------- |
| .text   | Executable code       | `mov x0, #1`          |
| .data   | Initialized data      | `my_var: .word 42`    |
| .bss    | Zero-initialized data | `buffer: .space 1024` |
| .rodata | Read-only data        | `msg: .asciz "Hello"` |




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Hello...but goodbye 0

:::
 

<code>
<pre>
.section .text
.global _start

_start:
    mov x8, #93          // Syscall number for exit (93)
    mov x0, #0           // Exit code (0)
    svc #0               // Make the syscall
</pre></code>



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Hello...but goodbye 1

:::
 

<code>
<pre>
.equ sys_exit, 93
.section .text
.global _start

_start:
    mov x8,  sys_exit          // Syscall number for exit (93)
    mov x0, #0           // Exit code (0)
    svc #0               // Make the syscall
    
</pre></code>

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Hello...but goodbye 2 

:::
 

<code>
<pre>
// Define sys_exit to be 93
.equ sys_exit, 93


// Macro to perform the exit syscall
// note that code is an arg
.macro exit code
    mov x8, sys_exit     // Load syscall number (94 for exit)
    mov x0, \code        // Load the exit code into x0
    svc #0               // Make the syscall
.endm

.section .text
.global _start


_start:
    exit 0               // Call the macro with exit code 0
    
</pre></code>



---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### include 

:::
::: left
<code>
<pre>
.include "common.S" // Include the macros file

.section .text
.global _start


_start:
    exit 0 
</pre>
</code>
:::
::: right 
<code>
<pre>
// common.S
// Define syscall numbers
.equ sys_exit, 93 // Syscall number for exit

// Macro to perform the exit syscall
.macro exit code
    mov x8, sys_exit     // Load syscall number
    mov x0, \code        // Load the exit code into x0
    svc #0               // Make the syscall
.endm
</pre>
</code>
:::
::: source
<small>
aarch64-linux-gnu-as -I ./src -o ./bin/exit.4.o src/exit.4.S
<br>
aarch64-linux-gnu-ld -o ./bin/exit ./bin/exit.4.o
<small>
:::


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Dynamic Linking

:::
- Assembly/compilation creates an object file 
- There are symbols that the linker needs to resolve
- for example `libc!printf`


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### call printf
:::
<code>
<pre>


/* build 
aarch64-linux-gnu-as -I ./src   -o ./bin/hello_printf.o src/hello_printf.S
aarch64-linux-gnu-ld -lc -o bin/hello_printf ./bin/hello_printf.o
*/
.include "common.S" // Include the macros file

.section .rodata 
message:
    .asciz "Hello from AArch64 assembly!" // Null-terminated string
fmt:
  .asciz "%s\n"


.section .text
.global _start


_start:
    ldr x0, =fmt 
    ldr x1, =message
    bl printf
    exit 0 

</pre>
</code>




---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Unconditional Branch Example

:::

```asm
sub:
    bl do_something   // call do_something()
    b end            //  jump over next block
    BL wont_execute  // won't be executed
end:
    ret
```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Conditional Branches

:::

- After an instruction that sets flags, you can do:
    - **B.EQ** if Z=1
    - **B.NE** if Z=0
    - **B.GT** , **B.LT** , etc. (these interpret flags N, Z, V)
- Example:
    
    ```asm
    subs x0, x1, #5     // x0 = x1 - 5, sets flags
    b.eq  label_eq
    b.gt  label_gt
    b label_else
    ```
    

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Compare & Branch Example

:::

```asm
cmp_loop:
    subs x1, x1, #1  ; x1 = x1 - 1
    cbz x1, done     ; if x1 == 0 => branch to done
    // do something else
    b cmp_loop

done:
    ret
```

- **CBZ xReg, label** => branch if `xReg == 0`.
- **CBNZ xReg, label** => branch if `xReg != 0`.

---
<!-- slide template="[[Base Slide]]" -->


::: title

#### Memory Access Instructions

:::


- **ldr** and **str**: Load/Store register from/to memory.
    - e.g., `ldr x0, [x1]` => x0 = _(uint64_t_)x1
- **ldur** and **stur**: “unscaled” versions (useful if the offset is larger or negative).
- **LDP** and **STP**: Load/Store Pair (two registers at once), typically used in function prologues/epilogues.





---
<!-- slide template="[[Base Slide]]" -->
::: title

#### LDR/STR Examples

:::

```asm
// Load x0 from memory at address x1
ldr x0, [x1]

// Store x2 into memory at address x3 + 0x10
str x2, [x3, #0x10]

// Load w4 from x5 + offset
ldr w4, [x5, #4]   ; 32-bit load into w4
```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### LDP/STP: Load/Store Pair

:::

- Load or store two registers at consecutive memory locations in one instruction:
    
    ```asm
    stp x0, x1, [x2, #-16]! 
    // store x0, x1 on the stack at (x2 - 16), updating x2
    
    ldp x19, x20, [sp], #16
    // load x19, x20 from [sp], then sp += 16
    ```
    
- Often used for function frames (saving/restoring registers).

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### The Stack in AArch64

:::

- Grows **downward** in memory.
- `SP` should always be 16-byte aligned.
- Common function prologue:
    
    ```asm
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    ```
    
- Epilogue:
    
    ```asm
    mov sp, x29
    ldp x29, x30, [sp], #16
    ret
    ```
    

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example: Simple Function

:::

```asm
// int sum_two(int a, int b) { return a + b; }

sum_two:
    add w0, w0, w1    ; w0 = w0 + w1
    ret
```

- `a` is in `w0`, `b` is in `w1`.
- Return in `w0`.
- On 64-bit side, `x0`, `x1`, etc.

---
<!-- slide template="[[Base Slide]]" -->
::: title

#### Example: Subroutine with Stack

:::
<code>
<pre>
sum_array:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    ; x0 = arr, x1 = len
    mov w2, #0         ; total = 0

loop:
    cbz w1, end        ; if len == 0 => end
    ldr w3, [x0], #4   ; w3 = *arr, arr++
    add w2, w2, w3     ; total += w3
    sub w1, w1, #1     ; len--
    b loop

end:
    mov w0, w2         // return total
    mov sp, x29
    ldp x29, x30, [sp], #16
    ret
</pre>
</code>


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Condition Codes Recap

:::

- **EQ / NE** (equal / not equal) => Z=1 or Z=0
- **GT / LT** (greater / less) => Based on N,V bits
- **GE / LE** => etc.
- **HI / LO** => For unsigned comparisons with carry.
- Master these for deeper control-flow constructs.

