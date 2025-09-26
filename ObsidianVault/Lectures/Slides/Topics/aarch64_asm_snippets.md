
---
<!-- slide template="[[Base Slide]]" -->

::: title

#### How do we learn A64 in this class? 

:::
- `objdump -d a.out`
- Writing little bootstraps 


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Baby's first `A64`

:::

```
.section .text
.global _start

_start:
    mov x8, #93          // Syscall number for exit (93)
    mov x0, #0           // Exit code (0)
    svc #0               // Make the syscall
```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Baby's first `A64`

:::

```
.equ sys_exit, 93


.section .text
.global _start

_start:
    mov x8, sys_exit          // Syscall number for exit (93)
    mov x0, #0           // Exit code (0)
    svc #0               // Make the syscall
```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Baby's first `A64`

:::

```
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

```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Baby's first `A64`

:::
```
// common.S
// Define syscall numbers
.equ sys_exit, 93 // Syscall number for exit

// Macro to perform the exit syscall
.macro exit code
    mov x8, sys_exit     // Load syscall number
    mov x0, \code        // Load the exit code into x0
    svc #0               // Make the syscall
.endm
```

```
.include "common.S" // Include the macros file

.section .text
.global _start


_start:
    exit 0 
```




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### hello world 

:::

- `write`: `fd=1`, `addr_str`, `str_size`

```
.section .rodata 
hello:
  .asciz "hello world!\n"

.section .text 
.global _start
_start:
  // 1 is stdout 
  mov x0, #1
  ldr x1, =hello 
  mov x2, #14
  mov x8, #64
  svc 0
  mov x0, 0
  mov x8, #93
  svc 0

```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### hello world 

:::
```
// Macros 
.equ STDOUT_FD, 1
.equ SYS_WRITE, 64
.equ  SYS_EXIT, 93


.section .rodata 
hello:
  .asciz "hello world!\n"

.section .text 
.global _start
_start:
  // 1 is stdout 
  mov x0, STDOUT_FD
  ldr x1, =hello 
  mov x2, #14
  mov x8, SYS_WRITE
  svc 0
  mov x0, 0
  mov x8, SYS_EXIT
  svc 0
```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### hello world 

:::
```
// Macros 
.equ STDOUT_FD, 1
.equ SYS_WRITE, 64
.equ  SYS_EXIT, 93

.macro write_stdout message,length
  mov x0, STDOUT_FD
  ldr x1, =\message 
  mov x2, \length 
  mov x8, SYS_WRITE
  svc 0
.endm

.macro exit_with code 
  mov x0, \code 
  mov x8, SYS_EXIT
  svc 0
.endm 

.section .rodata 
hello:
  .asciz "hello world!\n"

.section .text 
.global _start
_start:
  write_stdout hello, 14
  exit_with 0
```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### hello world 
:::
```
.equ  SYS_EXIT, 93
.section .rodata
fmt: .asciz "Hello %s!\n"
fmt_alt: .asciz "fmt addr: 0x%p\n"
val: .asciz "world"

.section .text 
.global _start 

_start:
  ldr x0, =fmt
  ldr x1, =val 
  bl printf 
  ldr x0, =fmt_alt
  ldr x1, =fmt 
  bl printf 
  mov x8, SYS_EXIT
  svc 0 
```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### goldbold

:::
- https://godbolt.org/z/Gxjor9Kqj

```c
#include <stdio.h>

unsigned long long  factorial(unsigned long long  n){
    if (n == 0){
        return 1;
    }
    return n * factorial(n-1);
}


int main(int argc, char** argv){
    unsigned long long out = factorial(5);
    printf("Out: %llu\n", out);
return 0;
}

```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### goldbolt

:::

```
factorial(unsigned long long):                          // @factorial(unsigned long long)
        cbz     x0, .LBB0_4
        mov     x8, x0
        mov     w0, #1
.LBB0_2:                                // =>This Inner Loop Header: Depth=1
        subs    x9, x8, #1
        mul     x0, x8, x0
        mov     x8, x9
        b.ne    .LBB0_2
        ret
.LBB0_4:
        mov     w0, #1
        ret
main:                                   // @main
        str     x30, [sp, #-16]!                // 8-byte Folded Spill
        adrp    x0, .L.str
        add     x0, x0, :lo12:.L.str
        mov     w1, #120
        bl      printf
        mov     w0, wzr
        ldr     x30, [sp], #16                  // 8-byte Folded Reload
        ret
.L.str:
        .asciz  "Out: %llu\n"
```