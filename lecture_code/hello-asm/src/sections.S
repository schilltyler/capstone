/* Example using all major sections */
.section .rodata
msg:    .ascii  "Hello World!\n"
len = . - msg           /* Calculate string length */

.align 4                /* 4-byte alignment */
pi:     .double 3.1415926535

.section .data
counter:    .word   0   /* 32-bit initialized counter */
status:     .byte   1   /* Single byte variable */

.section .bss
.lcomm buffer, 256      /* 256-byte uninitialized buffer */
.lcomm big_array, 4096  /* 4KB array in BSS */

.section .text
.global _start

_start:
    /* System call write(fd=1, buf=msg, count=len) */
    mov x0, #1          /* stdout */
    ldr x1, =msg        /* message address */
    ldr x2, =len        /* message length */
    mov x8, #64         /* syscall: write */
    svc #0

    /* Access data section variables */
    ldr x3, =counter
    ldr w4, [x3]        /* Load 32-bit value */
    add w4, w4, #1
    str w4, [x3]        /* Store back */

    /* Access BSS section buffer */
    ldr x5, =buffer
    mov w6, #'A'
    strb w6, [x5]       /* Store 'A' at buffer[0] */

    /* Exit system call */
    mov x0, #0          /* status */
    mov x8, #93         /* syscall: exit */
    svc #0
