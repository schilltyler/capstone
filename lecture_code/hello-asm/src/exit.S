.section .text
.global _start

_start:
    mov x8, #93          // Syscall number for exit (93)
    mov x0, #0           // Exit code (0)
    svc #0               // Make the syscall
